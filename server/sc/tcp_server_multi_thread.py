
# 接收来自客户端的TCP报文，然后直接返回。

import os
#from socket import *
import socket as sock
import threading
from enum import Enum

class TSockStatus(Enum):
    FSM_INIT = 0          # 子线程中中socket刚建立后的初始状态
    FSM_AUTH_RECVD = 1    # 收到了认证报文
    FSM_AUTH_SENT = 2     # 将认证结果发送给了客户端
    FSM_DATA_COMM = 3

def init_socket():
    SERVER_IP = '0.0.0.0'
    SERVER_PORT = 9999

    # set the default timeout time
    # sock.setdefaulttimeout(3)

    # create the socket
    sock_svr = sock.socket(sock.AF_INET, sock.SOCK_STREAM)

    sock_svr.bind((SERVER_IP, SERVER_PORT))
    sock_svr.listen()

    #while True:
    for tIdx in range(3):
        try:
            sock_conn, sock_addr = sock_svr.accept()
            print(sock_addr)

            p = threading.Thread(target = run_socket, args = (sock_conn, ))
            p.start()

        except Exception as e:
            print("Setup the server failed.")
            return

    sock_svr.close()

    pass

def run_socket(sock_conn_in):
    print("run_socket is activated.")
    instSock = TSockStatus(TSockStatus.FSM_INIT)

    # while True:
    for tIdx in range(1):

        if (TSockStatus.FSM_INIT == instSock):
            print("run_socket: FSM_INIT")
            # 初始状态下等待来自客户端的认证消息
            data = sock_conn_in.recv(128)
            print(data)
            sock_conn_in.send(data.upper())       

            instSock = TSockStatus.FSM_AUTH_RECVD
            pass
        elif (TSockStatus.FSM_AUTH_RECVD == instSock):
            print("run_socket: FSM_AUTH_RECVD")
            # 接收到认证报文的状态下向
            instSock = TSockStatus.FSM_INIT
            pass
        elif (TSockStatus.FSM_AUTH_SENT == instSock):
            pass
        elif (TSockStatus.FSM_DATA_COMM == instSock):
            pass

    #for tIdx in range(3):
    #    data = sock_conn_in.recv(128)
    #    print(data)
    #    sock_conn_in.send(data.upper())

    print("connection is lost.")
    sock_conn_in.close()
    pass

if "__main__" == __name__:
    os.system("cls")
    print("-----------------------------------------")

    init_socket()



