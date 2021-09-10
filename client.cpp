//�ĳ�_2019055078_��ä��
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>  // ����, �����쿡�� ����� �����ϴ� ���
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT      7777
// Socket ���� �Լ�
void ErrorHandling(const char*);
void InitConn();
void CloseConn();

// Socket ���� ���� ����
WSADATA wsaData;
SOCKET cSocket;
SOCKADDR_IN servAddr;
char message[10];

int playerNo = 0;

int main(int argc, char* argv[])
{
    InitConn();
    printf("Client\n");
    while (1) {
        char buf[255] = { 0 };
        printf("Insert :");
        gets_s(buf);
        //������ �޼��� ����
        int sendsize = send(cSocket, buf, strlen(buf), 0);
        if (sendsize <= 0)
            break;
        //������ ���� �޼��� ����
        int recvsize = recv(cSocket, buf, sizeof(buf), 0);
        if (recvsize <= 0) {

            break;
        }
        buf[recvsize] = '\0';
        printf("server >> %s\n", buf);
    }
    CloseConn();
    printf("��������\n");

    return 0;
}

void InitConn()
{
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        ErrorHandling("WSAStartup() error!");
    }

    cSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (INVALID_SOCKET == cSocket) {
        ErrorHandling("socket() error!");
    }

    // ���� ���� ���� �ʱ�ȭ
    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = PORT;

    // ����
    if (SOCKET_ERROR == connect(cSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))) {
        ErrorHandling("connect() error!");
    }
}

//����

void CloseConn()
{
    closesocket(cSocket);

    WSACleanup();
}


void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stdout);
    exit(-1);

}