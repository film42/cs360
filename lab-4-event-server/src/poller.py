import errno
import select
import socket
import sys
import traceback
from time import time

from controller import Controller
from config import Config


class Poller:
    """ Polling server """

    def __init__(self, port):
        self.host = ""
        self.port = port
        self.open_socket()
        self.clients = {}
        self.size = 1024

        self.config = Config("web.conf")

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
        while True:
            # poll sockets
            try:
                fds = self.poller.poll(timeout=self.config.timeout())
                self.close_idle_connections()
            except:
                return
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

    def handle_error(self, fd):
        self.poller.unregister(fd)
        if fd == self.server.fileno():
            # recreate server socket
            self.server.close()
            self.open_socket()
            self.poller.register(self.server, self.poll_mask)
        else:
            # close the socket
            self.clients[fd][0].close()
            del self.clients[fd]

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
            self.clients[client.fileno()] = (client, time())
            self.poller.register(client.fileno(), self.poll_mask)

    def handle_client(self, fd):

        print "Processing client!"
        try:
            # Reset the time for mark and sweep
            self.clients[fd] = (self.clients[fd][0], time())
            # Try to get data
            # TODO: Maker sure this will get the entire request
            data = self.clients[fd][0].recv(10000)
        except socket.error, (value, message):
            # if no data is available, move on to another client
            if value == errno.EAGAIN or errno.EWOULDBLOCK:
                return
            print traceback.format_exc()
            sys.exit()

        if data:
            print "Process moving to controller!"
            Controller(self.clients[fd][0], self.config).handle_request(data)
        else:
            self.poller.unregister(fd)
            self.clients[fd][0].close()
            del self.clients[fd]

        print "Done with socket: %d" % fd

    def close_idle_connections(self):
        """
        Method use to delete expired clients using the config's timeout function
        """
        for key in self.clients.keys():
            # Check if the mark time has expired
            if (time() - self.clients[key][1]) > self.config.timeout():
                print "Closing socket %d" % key
                self.handle_error(key)
