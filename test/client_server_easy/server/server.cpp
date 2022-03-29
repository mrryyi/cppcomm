#include "../../../src/cppcomm.hpp"
#include "../common.hpp"

int main(int argc, char* argv[]) {

  printf("Program Started: %s\n", argv[0]); 

  std::string inet_address = "127.0.0.1";

  // Forces stdout to be line-buffered.
  setvbuf(stdout, NULL, _IONBF, 0);

  //--------------------------------------------
  // Initialize Winsock
  int     iResult;
  WSADATA wsaData;
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      printf("WSAStartup failed: %d", iResult);
      exit(EXIT_FAILURE);
  }

  SOCKET sock;

  if( !cppcomm::make_socket( &sock ) )
  {
    printf( "socket failed: %d", WSAGetLastError() );
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  //----------------------------------------
  // Make local address
  SOCKADDR_IN local_address;
  local_address.sin_family = address_family;
  local_address.sin_port = htons( PORT_SERVER );
  local_address.sin_addr.s_addr = INADDR_ANY;
  
  //----------------------------------------
  // Bind the socket to local address and port.
  if( bind( sock, (SOCKADDR*)&local_address, sizeof( local_address ) ) == SOCKET_ERROR )
  {
    printf( "bind failed: %d", WSAGetLastError() );
    exit(EXIT_FAILURE);
  }
  
  printf("Running.\nWaiting for messages...\n");
  
  for (ever) {
    // Receive messages.
    cppcomm::Message recv_msg;

    int bytes_received = recvfrom(sock,
                                  (char *) recv_msg.buffer,
                                  1024,
                                  recv_msg.flags,
                                  (SOCKADDR*)&recv_msg.address,
                                  (int*) &recv_msg.address_size);
    
    if (bytes_received != SOCKET_ERROR) {
      printf("Received message.\n");
      switch(recv_msg.buffer[0]) {
        case 1:
          printf("first byte is a 1\n");
          break;
        case 2:
          printf("first byte is a 2\n");
          break;
        default:
          break;
      }
    }
    else {
      printf("SOCKET_ERROR");
    }

    Sleep(1000);
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}