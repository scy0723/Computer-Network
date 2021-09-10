//컴네_2019055078_신채영
#define _WINSOCK_DEPRECATED_NO_WARNINGS      // 소켓 버전을 업데이트함에 따라 이전에 사용되던 함수를 금지하는 것을 무시

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>  // 소켓, 윈도우에서 통신을 관장하는 헤더
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT      7777
#define MAX_CLIENT   4

void InitServer();
void CloseServer();
void ErrorHandling(const char*);   // 예외처리함수

WSADATA wsaData;
SOCKET servSock, clientSock[4];         // 디스크립터
SOCKADDR_IN servAddr, clientAddr[4];   // 주소 관련 구조체

int clientAddrLen = 0;   // 클라이언트 주소 정보를 가지고 있는 데이터의 길이


void __cdecl ClientRecvThread(void* args)
{
    SOCKET sock = (SOCKET)args;
    char b[256];
    while (1)
    {
        //클라이언트로부터 수신
        int recvsize = recv(sock, b, sizeof(b), 0);
        if (recvsize <= 0)
        {
            printf("접속종료\n");
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
    // 소켓 라이브러리 초기화 (버전, IP 형식(V4, V6)) ///////////////////////(1)
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        ErrorHandling("WSAStartup() error!");
    }
    //설치한다.//////////////////////////////////////////////////////////////(2)
    servSock = socket(AF_INET, SOCK_STREAM, 0);

    if (INVALID_SOCKET == servSock) {
        ErrorHandling("socket() error!");
    }

    // 소켓 정보 초기화    //////////////////////////////////////////////////(3)                 
    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);      // 서버 IP 초기화
    servAddr.sin_port = PORT;                     // 포트 초기화

    // 소켓에 IP 주소와 PORT 번호 결합 //////////////////////////////////////(4)
    if (SOCKET_ERROR == bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {
        ErrorHandling("bind() error");
    }

    // 클라이언트 연결 대기//////////////////////////////////////////////////(5)
    if (SOCKET_ERROR == listen(servSock, MAX_CLIENT)) {
        ErrorHandling("listen() error");
    }

    fputs("유저 접속 대기중...\n", stdout);

    clientAddrLen = sizeof(clientAddr[0]);   // 클라이언트의 주소 정보의 길이는 버전마다 상이하기 때문에 중요한 요소

    for (int i = 0; i < MAX_CLIENT; i++) {

        // 클라이언트 연결 수락//////////////////////////////////////////////(6)
        clientSock[i] = accept(servSock, (SOCKADDR*)&(clientAddr[i]), &clientAddrLen);

        if (INVALID_SOCKET == clientSock[i]) {
            ErrorHandling("accept() error");
        }
        else {
            printf("\n%d번 클라이언트 connection complete!\n", i + 1);
            printf("IP: %s\n Port: %d\n", inet_ntoa(clientAddr[i].sin_addr), clientAddr[i].sin_port);
          
            _beginthread(ClientRecvThread, NULL, (void*)clientSock[i]);

        }

    }
}

void CloseServer()//종료
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