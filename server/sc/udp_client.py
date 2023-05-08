
# 测试和接入服务器之间的往返时间

import os
from socket import *
import socket as sock
import struct
import datetime
import time

class CUDPServer:
    __LEN_RECV_BUF__ = 1024
    __SOCK_ADDR_PORT__ = ("127.0.0.1", 8888)

    def __init__(self):
        self.cSock = sock.socket(sock.AF_INET, sock.SOCK_DGRAM)
        self.cSock.connect(self.__SOCK_ADDR_PORT__)
        pass

    def udp_client(self):

        for ti in range(100):
            #l_msg = "hello world"
            #self.cSock.send(l_msg.encode("ASCII"))

            iStartTime = datetime.datetime.now().microsecond
            bStartTime = struct.pack("!I", iStartTime)

            self.cSock.send(bStartTime)
            
            l_msg, l_addr = self.cSock.recvfrom(self.__LEN_RECV_BUF__)
            #iEndTime = struct.unpack("!I", l_msg)
            iEndTime = datetime.datetime.now().microsecond

            #print(iStartTime)
            #print(iEndTime)

            if iEndTime >= iStartTime:
                iElapsedTime = (iEndTime - iStartTime) / 1000
            else:
                iElapsedTime = (iEndTime - iStartTime) / 1000 + 1000

            print("elapsed time = %d" % (iElapsedTime))


            #print(l_msg)
            #print(l_addr)

        self.cSock.close()


if "__main__" == __name__:
    os.system("cls")
    print("------------------ sock client -----------------------")

    inst_sock = CUDPServer()
    inst_sock.udp_client()

