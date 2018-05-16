import socket
import sys

HOST, PORT = "192.168.0.100", 8000
data = "gate" #"hello".join(sys.argv[1:])

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(data + "\n")

    # Receive data from the server and shut down
    received = sock.recv(1024)
finally:
    sock.close()

print "Sent:     {}".format(data)
print "Received: {}".format(received)



import socket
import threading
import tcp_handler

class ThreadedServer(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.host, self.port))

    def listen(self):
        self.sock.listen(5)
        while True:
            client, address = self.sock.accept()
            #client.settimeout(60)
            threading.Thread(target = self.listenToClient,args = (client,address)).start()

    def listenToClient(self, client, address):
        size = 1024
        print "created handler: {}".format(address)
        while True:
            try:
                data = client.recv(size)
                if data:
                    # Set the response to echo back the recieved data
                    print "recv: {}".format(data)
                    # call data parser

                    splitdata = data.split(",", 2)

                    # gate number?
                    if splitdata[0] == "GATE":
                        if splitdata[1] == "01":
                            print "GATE01 init on {}".format(address)
                        elif splitdata[1] == "02":
                            print "GATE02 init on {}".format(address)
                        elif splitdata[1] == "03":
                            print "GATE03 init on {}".format(address)
                        else:
                            print "GATE init failed.."

                    if splitdata[0] == "TAG":
                        if splitdata[1] == "01":
                            print "TAG from GATE 01.."
                        elif splitdata[1] == "02":
                            print "TAG from GATE 02.."
                        elif splitdata[1] == "03":
                            print "TAG from GATE 03.."
                        else:
                            print "No TAG found.."
                else:
                    raise error('Client disconnected')
            except:
                print "closed: {}".format(address)
                client.close()
                return False

if __name__ == "__main__":
    port_num = 8000
    server = ThreadedServer('',port_num)
    threading.Thread(target=server.listen()).start()

    parser = tcp_handler.DataParser()
    threading.Thread(target=parser.parser())

