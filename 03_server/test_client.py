import socket
import time

host = "192.168.0.101"
port = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2.connect((host, port))

while True:
    s.sendall('STX1,000000000000000000002000,-25ETX')
    s2.sendall('STX2,000000000000000000001000,-26ETX')
    time.sleep(1)



