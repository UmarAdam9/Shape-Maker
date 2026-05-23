#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#include<iostream>
#include<vector>
#include <thread>
#include <queue>
#include<windows.h>
#include<inttypes.h>
#include <tchar.h>
#include <winbase.h>
#include "socketLib.h"



struct sUDP_recv
{
    int m_id;

    queue<vector<char>>m_packet_queue;              //The packet queue which will be pushed into and popped out (so unbounded?)
    int m_packet_queue_size;

    HANDLE m_packetQueueMutex;                      //mutex for packet queue

    SOCKET m_ClientSock;                            //client (which we are)
    sockaddr_in m_ClientAddr;
    int m_clientPort ;                              //= 4991;
    char m_clientIP[15];                            // = "192.168.100.20";
    int m_status;                                   //for returning successful init of sockets

    sockaddr_in m_serverAddr;                       //server (which is providing data)
    int m_serverPort;                               //= 4990;
    char m_serverIP[15];                            // = "192.168.100.20";

    vector<char>m_recvBuff;                         //local recvBuffer for copying data into

    sUDP_recv(int id,int clientPort , char* clientIP , int serverPort , char* serverIP , int queueSize)
    {
        m_id = id;
        m_clientPort = clientPort;
        strcpy(m_clientIP,clientIP);

        m_packet_queue_size = queueSize;

        m_serverPort = serverPort;
        strcpy(m_serverIP,serverIP);

        //create socket for client and and socket address for the server
        m_status = CreateSocket(&m_ClientSock ,&m_ClientAddr, m_clientPort, m_clientIP, 0 );

        if(m_status == -1)
            return;

        m_status = CreateSockerAddress(&m_serverAddr,m_serverPort,m_serverIP);
        if(m_status == -1)
            return;

    }

    sUDP_recv(){};

};


void CreateRecieveThread(sUDP_recv &udpRecv , int recvLen)
{
    udpRecv.m_recvBuff.resize(recvLen);

    cout<<endl;
    if(udpRecv.m_ClientSock == INVALID_SOCKET)
        cout << "The socket for using recvfrom is invalid "<<endl;
    else
        cout << "The socket is ready for recvfrom "<<endl;
        cout << "Buffer size: "       << udpRecv.m_recvBuff.size() << endl;
        cout << "Requested recvLen: " << recvLen << endl;

    cout<<"running thread for thread "<< udpRecv.m_id<<"...."<<endl;

    while(1)
    {
          int ServerAddrLen = sizeof(udpRecv.m_serverAddr);

          int bytesReceived = recvfrom(udpRecv.m_ClientSock,  udpRecv.m_recvBuff.data() , recvLen, 0, (SOCKADDR*)&udpRecv.m_serverAddr,&ServerAddrLen);
          if (bytesReceived == SOCKET_ERROR)
            {
                cout<<"SOCKET_ERROR.."<< WSAGetLastError() <<endl;
                // Handle error
                cout<<" closing socket....."<<endl;
                closesocket(udpRecv.m_ClientSock);
                WSACleanup();
                break;
            }
           else if(bytesReceived > 0)
            {
                WaitForSingleObject(udpRecv.m_packetQueueMutex, INFINITE); //lock mutex
                std::vector<char> temp(udpRecv.m_recvBuff.begin(),udpRecv.m_recvBuff.begin() + bytesReceived); //creating a temp because of std::move below (gpt told me)

                if (udpRecv.m_packet_queue.size() < udpRecv.m_packet_queue_size)
                {
                    udpRecv.m_packet_queue.push(std::move(temp));
                }
//                else      //to ensure that the fifo always contains the latest packets
//                {
//                     udpRecv.m_packet_queue.pop();
//                     udpRecv.m_packet_queue.push(std::move(temp));
//                }

                ReleaseMutex(udpRecv.m_packetQueueMutex); //unlock mutex
            }
            else
            {
                 cout<<"got nothing..."<<endl;
            }
    }
}


//acquire the mutex lock and directly pop the packet queue into a render thread's local buffer
void getDataFromUDPThread(sUDP_recv &udpRecv , char* destBuffer , int destBufferLen)
{
       WaitForSingleObject(udpRecv.m_packetQueueMutex, INFINITE); //lock mutex
       if (!udpRecv.m_packet_queue.empty()) {
           // cout<<"packet queue is not empty"<<endl;
           memcpy(destBuffer , udpRecv.m_packet_queue.front().data(),destBufferLen);
           udpRecv.m_packet_queue.pop();
        }
        else
        {
            /** Dont do anything **/
            //cout<<"packet queue is empty for stream " <<udpRecv.m_id <<endl;
        }
       ReleaseMutex(udpRecv.m_packetQueueMutex); //unlock mutex
}


struct sApplication
{

public:

    vector<thread>m_udpRecvThread;        //app will have an array of udpRecv threads
    vector<sUDP_recv>m_udpRecvInfo;       //for every udpRecv thread there will be a udpRecv struct
    thread m_renderThread;                //render Thread
    virtual void m_RenderFunction() = 0;  //A generic function that has to be overridden which is run on a separate thread parallel to the udpRecv threads (currently will be used for rendering)
    int m_numThreads;                     //Number of udpRecv threads to be created



    int m_initApp(int num_streams , char* appIP, vector<int>streamPortVector , vector<char*>streamIpVector , int packetQueueSize){


       /********Start the winsock dll************/
       //=======================================//

        WSADATA wsadata;
        int wsaerr;
        WORD wVersionRequested = MAKEWORD(2, 2);
        wsaerr = WSAStartup(wVersionRequested, &wsadata);
        if(wsaerr != 0){
            cout<<"Winsock dll not found"<<endl;
            return -1;
        }else{
            cout<<"Winsock dll found"<<endl;
            cout<<"Status: "<<wsadata.szSystemStatus<<endl;
        }
        //=======================================//



        m_numThreads = num_streams;
        if(streamPortVector.size() != 2 * num_streams || streamIpVector.size() != num_streams )
        {
            cout<<"number of streams does not correlate with the streamPortVector or streamIpVector"<<endl;
            return -1;
        }


        else
        {
            m_udpRecvThread.resize(num_streams);
            m_udpRecvInfo.resize(num_streams);

            for(int i=0;i<num_streams;i++)
            {
                cout<<"trying to create client socket with IP : " << appIP<< " and port : " <<streamPortVector[i*2] <<endl;
                cout<<"trying to create Server socket address with IP : " << streamIpVector[i]<< " and port : " <<streamPortVector[(i*2)+1] <<endl;

                m_udpRecvInfo[i]= sUDP_recv(i,streamPortVector[i*2],appIP,streamPortVector[(i*2)+1],streamIpVector[i],packetQueueSize);
                if(m_udpRecvInfo[i].m_status == -1)
                    return -1;
            }
            return 0;
        }


    }
    void m_startApp(int recvBuffLen = 1500){

        /**
        m_renderThread = thread(m_RenderFunction);
        m_RenderFunction is not a regular function pointer
        It’s a member function pointer, which is incomplete without an object
        So the compiler can’t invoke it error
        **/



        for(int i =0;i<m_numThreads;i++)
        {
            m_udpRecvThread[i] = thread(CreateRecieveThread ,std::ref(m_udpRecvInfo[i]),recvBuffLen);   //study more about ref
        }

        m_renderThread = thread(&sApplication::m_RenderFunction , this);     //study more about "this"
        for(int i=0;i<m_numThreads;i++)
        {
            m_udpRecvThread[i].join();
        }
        m_renderThread.join();

    }

};


