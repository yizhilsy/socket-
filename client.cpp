/*
    client code
    Authur:Shiyu Lu
*/

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <WinSock2.h>
#include <ws2tcpip.h>
using namespace std;
int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    string username, password;
    cout << "Enter username and password (format: username,password): " << endl;
    cin >> username >> password;

    send(clientSocket, (username + "," + password).c_str(), (username + "," + password).size(), 0);
    char response[1024] = { 0 };
    recv(clientSocket, response, sizeof(response), 0);
    char connectstatus = response[0];
    cout << response;
    memset(response, 0, sizeof(response));
    recv(clientSocket, response, sizeof(response), 0);
    cout << response << endl;

    if (connectstatus == 'W') {
        cout << "***Enter 'que' to find who are onlinue; Enter 'chat' to chat;Enter 'exitmain' to quit...***" << endl;
        while (true) {
            string control;
            cin >> control;
            if (control == "que") {
                //向服务器发送请求查看当前在线的用户
                send(clientSocket, control.c_str(), control.size(), 0);
                char onlinueuserret[1024] = { 0 };
                recv(clientSocket, onlinueuserret, sizeof(onlinueuserret), 0);
                cout << "onlinue users:" << endl;
                cout << onlinueuserret;
            }
            else if (control == "exitmain") {
                //向服务器发送请求退出登录
                send(clientSocket, control.c_str(), control.size(), 0);
                char byeword[32] = { 0 };
                recv(clientSocket, byeword, sizeof(byeword), 0);
                cout << byeword << endl;
                break;
            }
            else if (control == "chat") {
                //向服务器发送请求聊天
                send(clientSocket, control.c_str(), control.size(), 0);
                cout << "Enter the user which you want to chat with" << endl;
                string targetusername;
                cin >> targetusername;
                send(clientSocket, targetusername.c_str(), targetusername.size(), 0);
                //接收服务器验证结果
                char issuccess[32] = { 0 };
                recv(clientSocket, issuccess, sizeof(issuccess), 0);
                string connectans(issuccess);
                cout << connectans << endl;
                if (connectans == "connect success") {  //与另外一个用户连接成功，开始通信！
                    cout << "Enter exityes to down the chat..." << endl;
                    while (true) {
                        string message; cin >> message;
                        if (message == "exityes") { //结束当前与用户的通信
                            //disconnect
                            send(clientSocket, message.c_str(), message.size(), 0);
                            //接受结束bye
                            char byeword[32] = { 0 };
                            recv(clientSocket, byeword, sizeof(byeword), 0);
                            string byetemp(byeword);
                            cout << byetemp << endl;
                            break;
                        }
                        else {
                            send(clientSocket, message.c_str(), message.size(), 0);
                        }
                    }
                }
                
            }
            

        }

    }
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
