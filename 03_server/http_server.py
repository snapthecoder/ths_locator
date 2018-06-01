#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from os import curdir, sep

import re



PORT_NUMBER = 8080


# This class will handles any incoming request from
# the browser
class myHandler(BaseHTTPRequestHandler):

    def posReader(self, filename):
        data1 = open(filename, "r")
        daten1 = data1.read()
        data1.close()

        values = [float(s) for s in re.findall(r"[-+]?\d*\.\d+|[-+]?\d+", daten1)]

        return values

    # Handler for the GET requests
    def do_GET(self):
        if self.path == "/":
            self.path = "/index.html"


        try:
            # Check the file extension required and
            # set the right mime type

            sendReply = False
            if self.path.endswith(".html"):
                mimetype = 'text/html'
                sendReply = True
            if self.path.endswith(".jpg"):
                mimetype = 'image/jpg'
                sendReply = True
            if self.path.endswith(".gif"):
                mimetype = 'image/gif'
                sendReply = True
            if self.path.endswith(".js"):
                mimetype = 'application/javascript'
                sendReply = True
            if self.path.endswith(".css"):
                mimetype = 'text/css'
                sendReply = True
            if self.path.endswith(".txt"):
                mimetype = 'text/plain'
                sendReply = True

            if sendReply == True:
                # construct html file

                htmlfile = open("upper.html", "r")

                uphtml = htmlfile.read()

                htmlfile.close()

                uphtml = uphtml + "<table><tr>  <th>EPC</th>" + \
                            "<th>Position</th>" + \
                            "<th>RSSI</th>" + \
                            "<th>Timestamp</th>" + \
                            "</tr>"
                epc = 1000
                while epc <= 6000 :
                    filename = "pos" + str(epc) + ".txt"
                    values = self.posReader(filename)
                    uphtml = uphtml + "<tr><td>" + str(values[0]) + "</td>" + \
                            "<td>" + str(values[1]) + "</td>" + \
                            "<td>" + str(values[2]) + "</td>" + \
                            "<td>" + str(values[3]) + "</td>" + "</tr>"
                    epc = epc + 1000

                uphtml = uphtml + "</table></body></html>"

                htmlfile = open("index.html", "w+")

                uphtml = htmlfile.write(uphtml)

                htmlfile.close()


                # Open the static file requested and send it
                f = open(curdir + sep + self.path)
                self.send_response(200)
                self.send_header('Content-type', mimetype)
                self.end_headers()
                self.wfile.write(f.read())
                f.close()
            return


        except IOError:
            self.send_error(404, 'File Not Found: %s' % self.path)


try:
    # Create a web server and define the handler to manage the
    # incoming request
    server = HTTPServer(('192.168.0.101', PORT_NUMBER), myHandler)
    print 'Started httpserver on port ', PORT_NUMBER

    # Wait forever for incoming htto requests
    server.serve_forever()

except KeyboardInterrupt:
    print '^C received, shutting down the web server'
    server.socket.close()
