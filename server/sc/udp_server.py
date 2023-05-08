
# 

import os
from socket import *
import socket as sock
import struct
import datetime
import time

class CUDPServer:
    __LEN_RECV_BUF__ = 1024
    __SOCK_ADDR_PORT__ = ("0.0.0.0", 8888)

    def __init__(self):
        self.cSock = sock.socket(sock.AF_INET, sock.SOCK_DGRAM)
        self.cSock.bind(self.__SOCK_ADDR_PORT__)
        pass

    def udp_server(self):

        #for ti in range(3):
        while True:
            l_msg, l_addr = self.cSock.recvfrom(self.__LEN_RECV_BUF__)
            #print(l_msg)
            #print(l_addr)

            self.cSock.sendto(l_msg, l_addr)

        self.cSock.close()
        print("Program finished.")

if "__main__" == __name__:
    os.system("clear")
    print("----------------- sock server ------------------------")

    inst_sock = CUDPServer()
    inst_sock.udp_server()

