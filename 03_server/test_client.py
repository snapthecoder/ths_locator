import socket
import time

host = "192.168.0.101"
port = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.connect((host, port))

while True:
    s.sendall('STX1,000000000000000000002000,-55ETX')
    s.sendall('STX1,000000000000000000002000,-56ETX')
    s.sendall('STX1,000000000000000000002000,-60ETX')
    time.sleep(1)
    s1.sendall('STX1,000000000000000000001000,-50ETX')
    s1.sendall('STX1,000000000000000000001000,-56ETX')
    s1.sendall('STX1,000000000000000000006000,-58ETX')
    s1.sendall('STX1,000000000000000000005000,-59ETX')
    s1.sendall('STX2,000000000000000000001000,-51ETX')
    s1.sendall('STX1,000000000000000000007000,-45ETX')
    s1.sendall('STX2,000000000000000000001000,-52ETX')
    time.sleep(3)
    s.sendall('STX1,000000000000000000003000,-55ETX')
    s.sendall('STX1,000000000000000000004000,-56ETX')
    s.sendall('STX1,000000000000000000002000,-60ETX')
    time.sleep(2)
    s.sendall('STX1,000000000000000000005000,-55ETX')
    s.sendall('STX1,000000000000000000005000,-56ETX')
    s.sendall('STX1,000000000000000000005000,-60ETX')
    time.sleep(2)
    s.sendall('STX2,000000000000000000005000,-55ETX')
    s.sendall('STX2,000000000000000000005000,-56ETX')
    s.sendall('STX2,000000000000000000005000,-60ETX')
    time.sleep(2)
