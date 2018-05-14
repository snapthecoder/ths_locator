class DataParser:

    def parser(data):

        splitdata = data.split(",", 1)

        #gate number?
        if  splitdata[1] == "GATE":
            if splitdata[2] == "01":
                print "GATE01 init.."
            elif splitdata[2] == "02":
                print "GATE02 init.."
            elif splitdata[2] == "03":
                print "GATE03 init.."
            else:
                print "GATE init failed.."







