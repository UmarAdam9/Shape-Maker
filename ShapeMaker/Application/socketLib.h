#define _WIN32_WINNT 0x0600

#include <winsock2.h>
#include <ws2tcpip.h>
#include<iostream>

#include<windows.h>
#include<inttypes.h>
#include <tchar.h>
#include <winbase.h>

using namespace std;
int CreateSocket(SOCKET* socket_in ,sockaddr_in* socket_addr_in, int port_in, char* ip_in, int udp_tcp_sel )
{
    *socket_in = INVALID_SOCKET;                       //create socket

    if(!udp_tcp_sel)
        *socket_in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //configure socket i.e UDP or TCP, local or over the Internet etc
    else
        *socket_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   //configure socket i.e UDP or TCP, local or over the Internet etc


    if(*socket_in == INVALID_SOCKET){
        cout<<"Error at socket():"<<WSAGetLastError()<<endl;
        WSACleanup();
        return -1;
    }else{
        cout<<"socket is OK"<<endl;
    }

    //port and IP configurations
    socket_addr_in->sin_family = AF_INET;
    socket_addr_in->sin_port = htons(port_in);
    //service.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces

    inet_pton(AF_INET, _T(ip_in),  (void*)&socket_addr_in->sin_addr.s_addr );


    if (bind(*socket_in, (SOCKADDR*)socket_addr_in, sizeof(*socket_addr_in)) == SOCKET_ERROR) {
        cout << "bind() failed:" << WSAGetLastError() << endl;
        closesocket(*socket_in);
        WSACleanup();
        return -1;
    }
    else {
        cout << "bind() is OK" << endl;
    }
    return 0;

}

int CreateSockerAddress(sockaddr_in* socket_addr_in, int port_in, char* ip_in)
{
    socket_addr_in->sin_family = AF_INET;
    inet_pton(AF_INET, _T(ip_in), (void*)&socket_addr_in->sin_addr.s_addr);
    socket_addr_in->sin_port = htons(port_in);
    //destination.sin_addr.s_addr = htonl(_T("192.168.100.67")); // Listen on all interfaces
    return 0;
}



#ifdef USE_FUNCS
void UDP_CreateSendSocket(SOCKET* sendSocket ,sockaddr_in* sendSocket_addr, int send_port, char* sendIP, sockaddr_in* recvSocket_addr, int recv_port, char* recvIP,char* sendBuf,int sendBuf_len,char foreverFlag, int timeout_ms)
{
    CreateSocket(sendSocket ,sendSocket_addr, send_port, sendIP, 0 );
    CreateSockerAddress(recvSocket_addr,recv_port,recvIP);
    if(foreverFlag)
    {
        while(1)
        {
          int bytesSent = sendto(*sendSocket, sendBuf, sendBuf_len, 0, (SOCKADDR*)recvSocket_addr, sizeof(*recvSocket_addr));
          if (bytesSent == SOCKET_ERROR)
          {
            cout<<"Error in bytesSend "<< WSAGetLastError() <<endl;
            // Handle error
            closesocket(*sendSocket);
            WSACleanup();
          }
          Sleep(timeout_ms);

        }
    }

    else
    {
        int bytesSent = sendto(*sendSocket, sendBuf, sendBuf_len, 0, (SOCKADDR*)recvSocket_addr, sizeof(*recvSocket_addr));
        if (bytesSent == SOCKET_ERROR)
        {
            cout<<"Error in bytesSend "<< WSAGetLastError() <<endl;
            // Handle error
            closesocket(*sendSocket);
            WSACleanup();
        }


    }

}

void UDP_CreateRecieveSocket(SOCKET* recieveSocket ,sockaddr_in* recieveSocket_addr, int recv_port, char* recvIP, sockaddr_in* senderSocket_addr, int sender_port, char* senderIP,vector<char>&recvBuf,int recvBuf_len)
{
    CreateSocket(recieveSocket ,recieveSocket_addr, recv_port, recvIP, 0 );
    CreateSockerAddress(senderSocket_addr,sender_port,senderIP);
    int sender_addr_len = sizeof(*senderSocket_addr);
    while(1)
    {
            vector<char>dataBuffer(recvBuf_len);

            int bytesReceived = recvfrom(*recieveSocket, dataBuffer.data() , recvBuf_len, 0, (SOCKADDR*)senderSocket_addr,&sender_addr_len);
            if (bytesReceived == SOCKET_ERROR)
            {
                //cout<<"error recieving.."<< WSAGetLastError() <<endl;
                // Handle error
                closesocket(*recieveSocket);
                WSACleanup();
            }
            else if(bytesReceived > 0){
               // cout<<bytesReceived<<endl;
                WaitForSingleObject(ghMutex, INFINITE); //lock mutex
                packet_queue.push(std::move(dataBuffer));
                //packet_queue.emplace(recvBuf.begin(), recvBuf.begin() + bytesReceived);
                ReleaseMutex(ghMutex); //unlock mutex
            }
            else
            {
                cout<<"got nothing..."<<endl;
            }


    }

}
#endif





