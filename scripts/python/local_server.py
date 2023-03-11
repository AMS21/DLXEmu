#!/usr/bin/env python3

import sys
from http.server import HTTPServer, SimpleHTTPRequestHandler, test
import http.server
import socketserver

PORT = 8000


class CORSRequestHandler (SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        SimpleHTTPRequestHandler.end_headers(self)


def run():
    server_address = ('', PORT)
    httpd = HTTPServer(server_address, CORSRequestHandler)
    httpd.serve_forever()


run()
