"""
A TCP echo client that allows the user to send multiple lines to the server.
Entering a blank line will exit the client.
"""

import argparse

import socket
import sys

class Client:
    """ Echo client """
    def __init__(self,host,port):
        self.host = host
        self.port = port
        self.size = 1024
        self.open_socket()
        print "Enter a blank line to stop."
        sys.stdout.write('> ')

    def open_socket(self):
        """ Connect to the server """
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.connect((self.host,self.port))
        except socket.error, (value,message):
            if self.server:
                self.server.close()
            print "Could not open socket: " + message
            sys.exit(1)

    def run(self):
        """ Read from the keyboard and send this line to the server """
        while True:
            # read from keyboard
            line = sys.stdin.readline()
            if line == '\n':
                break
            self.server.send(line)
            data = self.server.recv(self.size)
            sys.stdout.write(data)
            sys.stdout.write('> ')
        self.server.close()

class Main:
    """ Parse command line options and perform the download. """
    def __init__(self):
        self.parse_arguments()

    def parse_arguments(self):
        ''' parse arguments, which include '-p' for port '''
        parser = argparse.ArgumentParser(prog='Echo Server', description='A simple echo server that handles one client at a time', add_help=True)
        parser.add_argument('-s', '--server', type=str, action='store', help='host the server is running on',default='localhost')
        parser.add_argument('-p', '--port', type=int, action='store', help='port the server is bound to',default=3000)
        self.args = parser.parse_args()

    def run(self):
        c = Client(self.args.server,self.args.port)
        c.run()

if __name__ == "__main__":
    m = Main()
    m.parse_arguments()
    m.run()