
# 接收来自客户端的TCP报文，然后直接返回。

import os
#from socket import *
import socket as sock
import struct
import datetime

SERVER_IP = '0.0.0.0'
SERVER_PORT = 9999
BUFLEN = 128


# 响应客户端发送的报文，收到的内容全部返回。
def sock_response(sockIn):

    for  tidx in range (1, 100):
        # 构建用于测试请求的 echo 报文
        reqPacket = struct.pack("!B15sIHHHHHI", IOT_PKT_UP_DATA, __SN_BOARD__, __SEJILA_PRODUCT_ID__, __PRODUCT_VER__, 8, 0x0801, tidx, 1, 0)

        #print(reqPacket)
        #print(encode_msg(reqPacket))

        startTime = datetime.datetime.now()
        #print(startTime)

        # send connect request to the server
        sockIn.send(encode_msg(reqPacket))
        try:
            dataRecv = sockIn.recv(BUFLEN)
            if (len(dataRecv) > 0):
                endTime = datetime.datetime.now()
                elapsedTime = endTime - startTime
                print(elapsedTime)

                #print("received test response.")
                #print(dataRecv[4:-2])

                msg_decoded = decode_msg(dataRecv[4:-2])
                #print(msg_decoded)

                msg_unpacked = struct.unpack('!BHHHHI', msg_decoded)
                #print(msg_unpacked)

        except Exception as e:
            print("Recevie from the server time out.")
            continue

    return

def tcpclient():

    # set the default timeout time
    # sock.setdefaulttimeout(3)

    # create the socket
    sock_svr = sock.socket(sock.AF_INET, sock.SOCK_STREAM)

    while True:
        # connect to the server
        try:
            sock_svr.bind((SERVER_IP, SERVER_PORT))
            sock_svr.listen(1)
        except Exception as e:
            print("Setup the server failed.")
            return

        print("Ready to accept request from the client.")

        sock_client, addr_client = sock_svr.accept()

        print("Ready to receive data from the client.")

        while True:
            # 接收来自客户端的数据
            msg_fromClient = sock_client.recv(BUFLEN)
            if (len(msg_fromClient) > 0):
                #print(msg_fromClient)
                tickFromClient = struct.unpack('!I', msg_fromClient)
                #print(tickFromClient)
                sock_client.send(msg_fromClient)

            else:
                print("Receive again...")

    print("Server is setup.")
    

    sock_svr.close()

if "__main__" == __name__:
    os.system("cls")
    print("-----------------------------------------")

    tcpclient()

