#include "ipc.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "7777"

u8 recv_as_bytes(SOCKET s) {
  char recvc;
  char buffer[32] = {0};
  int offset = 0;
  for(;;) {
    int res = recv(s, &recvc, 1, 0);
    if (res > 0) {
      printf("Bytes received: %d | Offset: %d\n", res, offset);
      if(recvc == ' ') {
        return (u8)strtoi(buffer);
      }
      if(recvc >= 48 && recvc <= 57) {
        buffer[offset] = recvc;
        offset++;
      } else printf("invalid byte: %d\n", (int) recvc);
    }
    else if (res == 0) {
      printf("Connection closing...\n");
      return 255;
    }
    else  {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(s);
      WSACleanup();
      return 255;
    }
  }
}

int send_as_str(SOCKET s, u8 val) {
  char buffer[32] = {0};
  sprintf(buffer, "%d\n", (int)val);
  int bsent = 0;
  for(int i = strlen(buffer); bsent <= i;) {
    bsent += send(s, (const char*)buffer + bsent, strlen(buffer), 0);
    if (bsent == SOCKET_ERROR) {
      printf("send failed with error: %d\n", WSAGetLastError());
      closesocket(s);
      WSACleanup();
      return 0;
    }
  }
  return 1;
}

int start_host(void(*cb)(void *, Request *, Response *), void *env)
{
  WSADATA wsaData;
  int iResult;

  SOCKET ListenSocket = INVALID_SOCKET;
  SOCKET ClientSocket = INVALID_SOCKET;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int iSendResult;
  u8 recvbuf[DEFAULT_BUFLEN];
  int recvbuflen = DEFAULT_BUFLEN;
    
  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
  if ( iResult != 0 ) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  // Create a SOCKET for connecting to server
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  // Setup the TCP listening socket
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    printf("listen failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    printf("accept failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // No longer need server socket
  closesocket(ListenSocket);

  Request request;
  Response response;
  int offset = 0;
  u8 recvc = 0;
  // Receive until the peer shuts down the connection
  for(;;) {
    recvc = recv_as_bytes(ClientSocket);
    if (recvc < 255) {
      printf("Bytes received: %d | Offset: %d\n", iResult, offset);
      
      recvbuf[offset] = recvc;
      
      if(offset < REQUEST_SIZE) {
        offset++;
        continue;
      }
      offset = 0;

      memcpy(request.field, recvbuf, 200);
      memcpy(&request.t_piece, recvbuf + 200, 1);
      memcpy(&request.j_piece, recvbuf + 201, 1);
      memcpy(&request.z_piece, recvbuf + 202, 1);
      memcpy(&request.o_piece, recvbuf + 203, 1);
      memcpy(&request.s_piece, recvbuf + 204, 1);
      memcpy(&request.l_piece, recvbuf + 205, 1);
      memcpy(&request.i_piece, recvbuf + 206, 1);
      memcpy(&request.x_pos, recvbuf + 207, 1);
      memcpy(&request.y_pos, recvbuf + 208, 1);
      memcpy(&request.next_piece, recvbuf + 209, 1);
      memcpy(&request.drop_time, recvbuf + 210, 1);

      printf("Received Request\n");
            
      cb(env, &request, &response);

      if(!send_as_string(s, response.move)) return 1;
      if(!send_as_string(s, response.rotate)) return 1;

      printf("Sent Response\n");
      //response sent
    }
    else if (recvc == 255) {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
      return 1;
    }

  } 
  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
  }

  // cleanup
  closesocket(ClientSocket);
  WSACleanup();

  return 0;
}
