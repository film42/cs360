from http import HTTPRequest


class Controller:
    def __init__(self, socket, config):
        self.socket = socket
        self.config = config

    def handle_request(self, request):
        try:
            http_request = HTTPRequest(request)
            message = http_request.path
            self.response_ok(message)
        except:
            self.response_500()
            return

    def response_ok(self, message):
        self.response(200, "OK", message)

    def response(self, number, name, body=''):
        template = "HTTP/1.1 %d %s\r\n" % (number, name)
        template += "Content-Length: %d\r\n" % len(body)
        template += "Server: Hipster 0.0.1\r\n"
        template += "\r\n%s\r\n\r\n" % body

        # TODO: Make sure this accounts for failure to send
        self.socket.send(template)

    def response_400(self):
        self.response(400, "Bad Response")

    def response_403(self):
        self.response(403, "Forbidden")

    def response_404(self):
        self.response(404, "Not Found")

    def response_500(self):
        self.response(500, "Internal Server Error")

    def response_501(self):
        self.response(501, "Not Implemented")
