//�ĳ�_2019055078_��ä��
#define _WINSOCK_DEPRECATED_NO_WARNINGS      // ���� ������ ������Ʈ�Կ� ���� ������ ���Ǵ� �Լ��� �����ϴ� ���� ����

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>  // ����, �����쿡�� ����� �����ϴ� ���
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT      7777
#define MAX_CLIENT   4

void InitServer();
void CloseServer();
void ErrorHandling(const char*);   // ����ó���Լ�

WSADATA wsaData;
SOCKET servSock, clientSock[4];         // ��ũ����
SOCKADDR_IN servAddr, clientAddr[4];   // �ּ� ���� ����ü

int clientAddrLen = 0;   // Ŭ���̾�Ʈ �ּ� ������ ������ �ִ� �������� ����


void __cdecl ClientRecvThread(void* args)
{
    SOCKET sock = (SOCKET)args;
    char b[256];
    while (1)
    {
        //Ŭ���̾�Ʈ�κ��� ����
        int recvsize = recv(sock, b, sizeof(b), 0);
        if (recvsize <= 0)
        {
            printf("��������\n");
            break;
        }

        b[recvsize] = '\0';
        printf("client:  %s\n", b);
        int sendsize = send(sock, b, strlen(b), 0);
        if (sendsize <= 0)
            break;

    }

    closesocket(sock);

}

int main(int argc, char* argv[])
{
    InitServer();
    CloseServer();
    return 0;
}

void InitServer()
{
    // ���� ���̺귯�� �ʱ�ȭ (����, IP ����(V4, V6)) ///////////////////////(1)
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        ErrorHandling("WSAStartup() error!");
    }
    //��ġ�Ѵ�.//////////////////////////////////////////////////////////////(2)
    servSock = socket(AF_INET, SOCK_STREAM, 0);

    if (INVALID_SOCKET == servSock) {
        ErrorHandling("socket() error!");
    }

    // ���� ���� �ʱ�ȭ    //////////////////////////////////////////////////(3)                 
    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);      // ���� IP �ʱ�ȭ
    servAddr.sin_port = PORT;                     // ��Ʈ �ʱ�ȭ

    // ���Ͽ� IP �ּҿ� PORT ��ȣ ���� //////////////////////////////////////(4)
    if (SOCKET_ERROR == bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {
        ErrorHandling("bind() error");
    }

    // Ŭ���̾�Ʈ ���� ���//////////////////////////////////////////////////(5)
    if (SOCKET_ERROR == listen(servSock, MAX_CLIENT)) {
        ErrorHandling("listen() error");
    }

    fputs("���� ���� �����...\n", stdout);

    clientAddrLen = sizeof(clientAddr[0]);   // Ŭ���̾�Ʈ�� �ּ� ������ ���̴� �������� �����ϱ� ������ �߿��� ���

    for (int i = 0; i < MAX_CLIENT; i++) {

        // Ŭ���̾�Ʈ ���� ����//////////////////////////////////////////////(6)
        clientSock[i] = accept(servSock, (SOCKADDR*)&(clientAddr[i]), &clientAddrLen);

        if (INVALID_SOCKET == clientSock[i]) {
            ErrorHandling("accept() error");
        }
        else {
            printf("\n%d�� Ŭ���̾�Ʈ connection complete!\n", i + 1);
            printf("IP: %s\n Port: %d\n", inet_ntoa(clientAddr[i].sin_addr), clientAddr[i].sin_port);
          
            _beginthread(ClientRecvThread, NULL, (void*)clientSock[i]);

        }

    }
}

void CloseServer()//����
{
    for (int idx = 0; idx < MAX_CLIENT; idx++) {
        closesocket(clientSock[idx]);
    }

    closesocket(servSock);

    WSACleanup();
}

void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stdout);
    exit(-1);

}