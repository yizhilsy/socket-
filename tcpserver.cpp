#include <iostream>
#include <string>
#include <vector>;
#include <WinSock2.h>
#include <thread>
using namespace std;
struct User {
    string username;
    string password;
    SOCKET socket;
};
struct RegUser {
    string username;
    string password;
};
vector<User> onlineusers;
vector<RegUser> registedusers;

void init();
void NewRegUser(string& username,string& password);
void AddUser(string username, string password, SOCKET socket);  //��������û�
bool Authenticate(string username, string password);
void removeUser(string username, string password, SOCKET socket);   //ǿ������ĳЩ�����û�
void questiononlinueuser(string& ans) {
    for (int i = 0; i < onlineusers.size(); i++) {
        ans += onlineusers[i].username; ans += ", "; ans += onlineusers[i].password;
        ans += '\n';
    }
}
bool isexist(string& s) {
    for (int i = 0; i < onlineusers.size(); i++) {
        if (onlineusers[i].username == s) {
            return true;
        }
    }
    return false;//û���ҵ���ǰ���ߵ��û�
}

//������߳�
void clientHandler(SOCKET clientSocket);

int main() {
    init();
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // �����׽���
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // ���׽���
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // ��������
    listen(serverSocket, SOMAXCONN);

    cout << "Server is listening on port 8080..." << endl;

    while (true) {
        // ��������
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        thread(clientHandler, clientSocket).detach();
    }

    // �ر��׽���
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
void clientHandler(SOCKET clientSocket) {
    // �ӿͻ��˽�����Ϣ
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    string loginInfo(buffer);
    string username = loginInfo.substr(0, loginInfo.find(","));
    string password = loginInfo.substr(loginInfo.find(",") + 1, loginInfo.size());
    cout <<"���ڽ��ܵ����˺ţ�" << username << "," << password << endl;

    //��ݺ���
    if (Authenticate(username, password)) {
        send(clientSocket, "Welcome!", 8, 0);
        send(clientSocket, username.c_str(), username.size(), 0);
        //��¼�ɹ�����������û�����
        AddUser(username, password, clientSocket);
        while (true) {
            char control[128] = { 0 };
            recv(clientSocket, control, sizeof(control), 0);
            string nextstep(control);
            cout << nextstep << endl;
            if (nextstep == "que") {
                string ans;
                questiononlinueuser(ans);
                send(clientSocket, ans.c_str(), ans.size(), 0);
            }
            else if (nextstep == "exitmain") {
                //�ͻ�����ֹ����
                string byemessage = "Bye, User " + username;
                send(clientSocket, byemessage.c_str(), byemessage.size(), 0);
                removeUser(username, password, clientSocket);
                closesocket(clientSocket);
                cout << "User " << username << " logout..." << endl;
                return; break;
            }
            else if (nextstep == "chat") {
                //������������Ķ�������֤�Ƿ�ƥ��
                cout << "yesyesyes" << endl;
                char recvtarget[128] = { 0 } ;
                recv(clientSocket, recvtarget, sizeof(recvtarget), 0);
                cout << recvtarget << endl;
                string wantuser(recvtarget);
                cout << username << "want chat with " << wantuser << endl;
                if (isexist(wantuser)) {
                    //�����������
                    string issuccess="connect success";
                    send(clientSocket, issuccess.c_str(), issuccess.size(), 0);
                    
                    while (true) {  //����ѭ��ͨ��
                        memset(buffer, 0, sizeof(buffer));
                        recv(clientSocket, buffer, sizeof(buffer), 0);
                        string recmessage(buffer);
                        cout << recmessage << endl;
                        if (recmessage == "exityes") {  //�û��������ߣ�����������������Ƴ�
                            //���ͽ�������
                            string byemessage = "Bye, User " + username;
                            send(clientSocket, byemessage.c_str(), byemessage.size(), 0);
                            removeUser(username, password, clientSocket);
                            closesocket(clientSocket);
                            cout << "User " << username << " logout..." << endl;
                            break;
                        }
                        else {
                            //ʵʩ����ͨ��
                            cout << "User " << username << " Send: " << recmessage << endl;

                        }
                    }


                }
                else {
                    string issuccess = "connect fail! no such user!";
                    send(clientSocket, issuccess.c_str(), issuccess.size(), 0);
                }

            }
        }
        
    }
    else {
        cout << "�˺Ż���������!" << endl;
        send(clientSocket, "Authentication failed.Please login again.", 50, 0);
        closesocket(clientSocket);
    }
}

void init() {   //��ʼ�����û�
    RegUser user1; user1.username = "yizhilsy"; user1.password = "Lsy200304144815";
    RegUser user2; user2.username = "Greg"; user2.password = "0625";    
    RegUser user3; user3.username = "Roy064"; user3.password = "cheemsdogteam";
    registedusers.push_back(user1); registedusers.push_back(user2); registedusers.push_back(user3);
}
void AddUser(string username, string password, SOCKET socket) {
    User user; user.username = username; user.password = password;
    user.socket = socket;
    onlineusers.push_back(user);
}
bool Authenticate(string username, string password) {
    for (int i = 0; i < registedusers.size(); i++) {
        if (registedusers[i].username == username && registedusers[i].password == password) {
            return true;    //��ݺ���ɹ�
        }
    }
    return false;   //��ݷǷ�
}
void removeUser(string username, string password, SOCKET socket) {  //ǿ������ĳЩ�����û�
    for (int i = 0; i < onlineusers.size(); i++) {
        if (onlineusers[i].username == username && onlineusers[i].password == password && onlineusers[i].socket == socket) {
            vector<User>::iterator it = onlineusers.begin() + i;
            onlineusers.erase(it);  //ǿ������ĳЩ�����û�
            break;
        }
    }
}
void NewRegUser(string& username, string& password) {
    RegUser newclient; newclient.username = username; newclient.password = password;
    registedusers.push_back(newclient);
}