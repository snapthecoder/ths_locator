import socket
import threading
import SocketServer
import data_parser


class ClientThread(threading.Thread):

    def __init__(self,ip,port,clientsocket,dataParser):
        threading.Thread.__init__(self)
        self.ip = ip
        self.port = port
        self.csocket = clientsocket
        self.parser = dataParser
        print "[+] New thread started for "+ip+":"+str(port)

    def run(self):
        print "Connection from : "+ip+":"+str(port)

        data = "nodowhile"

        while len(data):
            try:
                data = self.csocket.recv(2048)
                # parse data
                self.parser.parse(data)
                print "GATE(%s:%s) sent : %s"%(self.ip, str(self.port), data)
            except socket.error, e:
                print "Socket error: %s" % e

        print "Client at "+self.ip+" disconnected..."


if __name__ == "__main__":

    host = "192.168.0.102"
    port = 8000

    try:
        tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tcpsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        tcpsock.bind((host, port))
    except socket.error , e:
        print "Socket error: %s" % e

    dataparser = data_parser.DataParser()

    while True:
        tcpsock.listen(4)
        print "Listening for incoming connections.."

        (clientsock, (ip, port)) = tcpsock.accept()

        newthred = ClientThread(ip, port, clientsock, dataparser)
        newthred.start()