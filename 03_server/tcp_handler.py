class DataParser:

    def parser():

        splitdata = ""#data.split(",", 2)

        print "i'm running too"

        # gate number?
        if splitdata[0] == "GATE":
            if splitdata[1] == "01":
                print "GATE01 init.."
            elif splitdata[1] == "02":
                print "GATE02 init.."
            elif splitdata[1] == "03":
                print "GATE03 init.."
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






