"""
A TCP echo server that handles multiple clients with polling.  Typing
Control-C will quit the server.
"""

import argparse

from poller import Poller


class Main:
    """ Parse command line options and perform the download. """

    def __init__(self):
        self.args = None
        self.parse_arguments()

    def parse_arguments(self):
        """ parse arguments, which include '-p' for port """
        parser = argparse.ArgumentParser(prog='Hipster Server 0.0.1',
                                         description='A simple event based web server built on epoll',
                                         add_help=True)
        parser.add_argument('-p', '--port', type=int, action='store', help='port the server will bind to', default=3000)
        self.args = parser.parse_args()

    def run(self):
        p = Poller(self.args.port)
        p.run()


if __name__ == "__main__":
    m = Main()
    m.parse_arguments()
    try:
        m.run()
    except KeyboardInterrupt:
        pass
