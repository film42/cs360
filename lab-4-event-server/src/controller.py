from http import HTTPRequest
from email import Utils
import socket as std_socket
import os
import datetime
import time
import errno


class Controller:
    def __init__(self, socket, config, cache, fd=0):
        self.socket = socket
        self.config = config
        self.cache = cache
        self.fd = fd

    def send(self, data):
        self.cache.set_cache(self.fd, data)

        total_bytes = len(data)
        bytes_sent_acc = 0
        while bytes_sent_acc < total_bytes:
            try:
                sent_bytes = self.socket.send(data[bytes_sent_acc:])
                bytes_sent_acc += sent_bytes

                self.cache.set_cache(self.fd, data[bytes_sent_acc + sent_bytes:])
            except std_socket.error as (error_number, message):
                if error_number == std_socket.EINTR:
                    continue

    def handle_request(self, request):
        try:
            http_request = HTTPRequest(request)

            # Check request params first
            if not http_request.valid:
                self.response_400()
                return

            # Check the method to make sure it's only GET
            if http_request.command != 'GET':
                self.response_501()
                return

            if 'host' in http_request.headers:
                host = http_request.headers['host'].split(':')[0]
                abs_path = self.config.absolute_path(http_request.path, host=host)
            else:
                abs_path = self.config.absolute_path(http_request.path)

            # Open file and send data
            with open(abs_path, 'r') as f:
                contents = f.read()
                size = f.tell()

            # Set additional headers
            headers = {
                'Content-Type': self.config.content_type(abs_path),
                'Last-Modified': self.last_modified_date(abs_path),
            }

            self.response_ok(contents, headers=headers, body_size=size)
        except (OSError, IOError) as e:
            if e.errno == errno.ENOENT:
                self.response_404()
            elif e.errno == errno.EACCES:
                self.response_403()
        except Exception as body:
            self.response_500('Controller: ' + body.message)
            return

    def rfc822_timestamp(self, dt):
        return Utils.formatdate(time.mktime(dt.timetuple()))

    def last_modified_date(self, filename):
        t = os.path.getmtime(filename)
        dt = datetime.datetime.fromtimestamp(t)
        return self.rfc822_timestamp(dt)

    def response_ok(self, message, headers={}, body_size=0):
        self.response(200, "OK", message, headers=headers, body_size=body_size)

    def response(self, number, name, body='', body_size=0, headers={}):
        if body_size == 0:
            body_size = len(body)

        template = "HTTP/1.1 %d %s\r\n" % (number, name)
        template += "Date: %s\r\n" % self.rfc822_timestamp(datetime.datetime.now())
        template += "Server: Hipster 0.0.1\r\n"
        template += "Content-Length: %d\r\n" % body_size

        # Allow for extra headers in the response
        for k, v in headers.iteritems():
            template += "%s: %s\r\n" % (k, v)

        template += "\r\n%s" % body

        self.send(template)

    def response_400(self):
        headers = {'Content-Type': 'text/plain'}
        self.response(400, "Bad Response", "Bad Response", headers=headers)

    def response_403(self):
        headers = {'Content-Type': 'text/plain'}
        self.response(403, "Forbidden", "Forbidden", headers=headers)

    def response_404(self):
        headers = {'Content-Type': 'text/plain'}
        self.response(404, "Not Found", "Not Found", headers=headers)

    def response_500(self, body=''):
        headers = {'Content-Type': 'text/plain'}
        self.response(500, "Internal Server Error", body, headers=headers)

    def response_501(self):
        headers = {'Content-Type': 'text/plain'}
        self.response(501, "Not Implemented", "Not Implemented", headers=headers)
