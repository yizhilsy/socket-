#include "dialog.h"
#include <QApplication>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <QString>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

//独立线程不断接受来自其他用户的信息
bool IsContinue = true;
void ReceiveMessage(SOCKET clientSocket_oc) {
    while (IsContinue) {
        char receivebuffer[1024] = {0};
        recv(clientSocket_oc, receivebuffer, sizeof(receivebuffer), 0);
        string recs(receivebuffer);
        QString anothers=QString::fromStdString(recs);
        cout << receivebuffer << endl;
    }
    cout << "用户下线，接受其他用户信息的线程销毁" << endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //SOCKET网络初始化
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //此SOCKET只接受服务端自有信息
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    //此SOCKET只接受其他客户端发送的信息
    SOCKET clientSocket_oc = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    ::connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    sockaddr_in serverAddr2;
    serverAddr2.sin_family = AF_INET;
    serverAddr2.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr2.sin_addr);
    ::connect(clientSocket_oc, (struct sockaddr*)&serverAddr2, sizeof(serverAddr2));

    //创建线程
    thread(ReceiveMessage, clientSocket_oc).detach();

    Dialog w(clientSocket,clientSocket_oc);
    w.show();

    //结束第二个线程
    IsContinue=false;


    return a.exec();
}
