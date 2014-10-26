import errno
import select
import socket
import sys
import traceback
from time import time

from controller import Controller
from config import Config
from cache import ResponseCache


class Poller:
    """ Polling server """

    def __init__(self, port):
        self.host = ""
        self.port = port
        self.open_socket()
        self.clients = {}
        self.size = 1024

        self.config = Config("web.conf")
        self.cache = ResponseCache()
        self.request_cache = {}

    def open_socket(self):
        """ Setup the socket for incoming clients """
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server.bind((self.host, self.port))
            self.server.listen(5)
            self.server.setblocking(0)
        except socket.error, (value, message):
            if self.server:
                self.server.close()
            print "Could not open socket: " + message
            sys.exit(1)

    def run(self):
        """ Use poll() to handle each incoming client."""
        self.poller = select.epoll()
        self.poll_mask = select.EPOLLIN | select.EPOLLHUP | select.EPOLLERR
        self.poller.register(self.server, self.poll_mask)

        # Mark and sweep timer
        last_run = time()

        while True:
            # poll sockets
            try:
                fds = self.poller.poll(timeout=self.config.timeout())
            except:
                return

            # Handle any waiting connections
            for (fd, event) in fds:
                # handle errors
                if event & (select.POLLHUP | select.POLLERR):
                    self.handle_error(fd)
                    continue
                # handle the server socket
                if fd == self.server.fileno():
                    self.handle_server()
                    continue
                # handle client socket
                self.handle_client(fd)

            # Send anything sitting in cache now
            for fd in self.cache.keys():
                controller = Controller(self.clients[fd][0], self.config, self.cache, fd=fd)
                controller.send(self.cache.get_cache(fd))

            # Timeout any inactive connections since last check
            if (time() - last_run) >= self.config.timeout():
                self.close_idle_connections()
                last_run = time()

    def close_client(self, fd):
        # close the socket
        self.poller.unregister(fd)
        self.clients[fd][0].close()
        del self.clients[fd]
        self.cache.del_cache(fd)

    def handle_error(self, fd):
        self.poller.unregister(fd)
        if fd == self.server.fileno():
            # recreate server socket
            self.server.close()
            self.open_socket()
            self.poller.register(self.server, self.poll_mask)
        else:
            self.close_client(fd)

    def handle_server(self):
        # accept as many clients are possible
        while True:
            try:
                (client, address) = self.server.accept()
            except socket.error, (value, message):
                # if socket blocks because no clients are available,
                # then return
                if value == errno.EAGAIN or errno.EWOULDBLOCK:
                    return
                print traceback.format_exc()
                sys.exit()
            # set client socket to be non blocking
            client.setblocking(0)
            self.clients[client.fileno()] = [client, False]
            self.poller.register(client.fileno(), self.poll_mask)

    def read_request(self, fd):
        return self.clients[fd][0].recv(10000)

    def handle_client(self, fd):
        try:
            controller = Controller(self.clients[fd][0], self.config, self.cache, fd=fd)

            self.clients[fd][1] = True
            # Try to get data
            data = self.read_request(fd)

            if fd not in self.request_cache:
                self.request_cache[fd] = data
            else:
                self.request_cache[fd] += data

            if "\r\n\r\n" not in self.request_cache[fd]:
                return

        except socket.error, (value, message):
            # if no data is available, move on to another client
            if value == errno.EAGAIN or errno.EWOULDBLOCK:
                return
            print traceback.format_exc()
            sys.exit()

        if fd in self.request_cache:
            controller.handle_request(self.request_cache[fd])
            del self.request_cache[fd]
        else:
            self.close_client(fd)

    def close_idle_connections(self):
        """
        Method use to delete expired clients using the config's timeout function
        """
        for key in self.clients.keys():
            # Delete if a client has not been marked as True since last tick
            if not self.clients[key][1]:
                self.close_client(key)
            else:
                # Mark socket for removal
                self.clients[key][1] = False
