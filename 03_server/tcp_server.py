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

                    splitdata = data.split(",", 1)

                    # gate number?
                    #if splitdata[1] == "GATE":
                    #    if splitdata[2] == "01":
                    #        print "GATE01 init.."
                    #    elif splitdata[2] == "02":
                    #        print "GATE02 init.."
                    #    elif splitdata[2] == "03":
                    #        print "GATE03 init.."
                    #    else:
                    #        print "GATE init failed.."
                else:
                    raise error('Client disconnected')
            except:
                print "closed: {}".format(address)
                client.close()
                return False

if __name__ == "__main__":
    port_num = 8000
    ThreadedServer('',port_num).listen()