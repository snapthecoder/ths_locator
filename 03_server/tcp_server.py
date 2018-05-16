import socket
import threading
import SocketServer
import re
import csv

class DataParser():

    # array pos = epc
    positions = [0, 0, 0, 0, 0, 0];

    def changePos(self, epc, gate):

        r = csv.reader(open('positions.txt'))  # Here your csv file
        lines = list(r)

        i = 0

        for row in lines:
            if row[0] == str(epc):
                if row[1] < str(gate):
                        lines[i][1] = str(gate)
                elif row[1] >= str(gate):
                        lines[i][1] = str(gate-1)
            i = i+1

        writer = csv.writer(open('positions.txt', 'w'))
        writer.writerows(lines)


    def parse(self, message):
        # system of a message
        # STX (start of text)
        # 2 chars for GATE Nr. (ex. 01)
        # comma
        # 24 chars for EPC (ex. 000000000000000000000001)
        # comma
        # 3 chars for RSSI (ex. -24)
        # ETX (end of text)

        # extract values from message

        values = [int(s) for s in re.findall(r"[-+]?\d*\.\d+|[-+]?\d+",message)]

        self.changePos(values[1], values[0])


class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        data = self.request.recv(1024)
        print "Recv:", data

        # splitdata = data.split(",", 3)

        # [int(splitdata) for splitdata in data.split() if splitdata.isdigit()]

        parser1.parse(data)


class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


def client(ip, port, message):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))
    try:
        sock.sendall(message)
        response = sock.recv(1024)
        print "Received: {}".format(response)
    finally:
        sock.close()


if __name__ == "__main__":
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "localhost", 8000

    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    ip, port = server.server_address

    # Start a thread with the server -- that thread will then start one
    # more thread for each request
    server_thread = threading.Thread(target=server.serve_forever)

    # Exit the server thread when the main thread terminates
    server_thread.daemon = True
    server_thread.start()

    print "Server loop running in thread:", server_thread.name

    parser1 = DataParser()

    while True:
        pass

    #client(ip, port, "Hello World 1")
    #server.shutdown()
    #server.server_close()


