import re
import csv
import os
import time
import threading
import tcp_server

class DataParser():

    def changePos(self, rssi, epc, gate):

        ts = time.time()
        i = 0

        #lock1.acquire()

        filename = "pos" + str(epc) + ".txt"

        if os.path.exists(filename) != True:
            csvfile = open(filename,"w+")
            csvfile.write(str(epc) + "," + str(gate)+ "," + str(rssi)+ "," + str(ts))
            csvfile.close()


        csvfile = open(filename, "r+")
        r = csv.reader(csvfile)

        lines = list(r)

        for row in lines:
            try:
                if row[0] == str(epc):
                    row[3] = ts
                    lines[i][1] = str(gate)
                    lines[i][2] = str(rssi)
                    # TODO read RSSI for direction rec.
                    i = i+1
            except IndexError, e:
                print "IndexError"

        writer = csv.writer(open(filename, 'w'))
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


        self.changePos(values[2],values[1], values[0])

        with open("log.txt", "a") as myfile:
            myfile.write(str(values[0])+",")
            myfile.write(str(values[1])+",")
            myfile.write(str(values[2])+",")
            myfile.write(time.strftime('%X %x %Z') + "\n")


