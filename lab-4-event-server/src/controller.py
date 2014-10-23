from http import HTTPRequest
import socket as std_socket


class Controller:
    def __init__(self, socket, config):
        self.socket = socket
        self.config = config

    def handle_request(self, request):
        try:
            http_request = HTTPRequest(request)
            abs_path = self.config.absolute_path(http_request.path)
            headers = {
                'Content-Type': self.config.content_type(abs_path)
            }

            # Open file and send data
            with open(abs_path, 'r') as f:
                contents = f.read()
                size = f.tell()

            self.response_ok(contents, headers=headers, body_size=size)
        except Exception as body:
            self.response_500('Controller: ' + body.message)
            return

    def response_ok(self, message, headers={}, body_size=0):
        self.response(200, "OK", message, headers=headers, body_size=body_size)

    def response(self, number, name, body='', body_size=0, headers={}):
        if body_size == 0:
            body_size = len(body)

        template = "HTTP/1.1 %d %s\r\n" % (number, name)
        template += "Content-Length: %d\r\n" % body_size
        template += "Server: Hipster 0.0.1\r\n"

        # Allow for extra headers in the response
        for k, v in headers.iteritems():
            template += "%s: %s\r\n" % (k, v)

        header_byte_len = len(template)

        template += "\r\n%s\r\n\r\n" % body

        # TODO: Be absolutely sure that this handles all errors: It doesn't!
        total_bytes = body_size + header_byte_len + 6
        bytes_sent_acc = 0
        while bytes_sent_acc < total_bytes:
            try:
                sent_bytes = self.socket.send(template[bytes_sent_acc:])
                bytes_sent_acc += sent_bytes
            except std_socket.error as (error_number, message):
                if error_number == std_socket.EINTR:
                    continue

    def response_400(self):
        self.response(400, "Bad Response")

    def response_403(self):
        self.response(403, "Forbidden")

    def response_404(self):
        self.response(404, "Not Found")

    def response_500(self, body=''):
        self.response(500, "Internal Server Error", body)

    def response_501(self):
        self.response(501, "Not Implemented")
