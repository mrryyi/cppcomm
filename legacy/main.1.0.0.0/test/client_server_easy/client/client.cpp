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
  {
    srand( (unsigned) time(NULL) );
    u_short random_port = 1000 + rand() % 56000;
  }
    local_address.sin_port = htons( PORT_CLIENT);
  local_address.sin_addr.s_addr = INADDR_ANY;
  
  //----------------------------------------
  // Bind the socket to local address and port.
  if( bind( sock, (SOCKADDR*)&local_address, sizeof( local_address ) ) == SOCKET_ERROR )
  {
    printf( "bind failed: %d", WSAGetLastError() );
    exit(EXIT_FAILURE);
  }

  //----------------------------------
  // Create an address to send messages to.
  SOCKADDR_IN server_address;
  server_address.sin_family = address_family;
  server_address.sin_port = htons( PORT_SERVER );
  server_address.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1"/*inet_address.c_str()*/ );
  
  printf("Running.\nSending messages...\n");
  
  for (ever) {
    printf("Sending message with byte 0 containing 1\n");
    {
      cppcomm::Message msg;
      uint32 msg_size = cppcomm::client_msg_one_write(msg.buffer);
      cppcomm::send_msg(&sock, msg, msg_size, server_address);
      Sleep(1000);
    }

    printf("Sending message with byte 0 containing 2\n");
    {
      cppcomm::Message msg;
      uint32 msg_size = cppcomm::client_msg_two_write(msg.buffer);
      cppcomm::send_msg(&sock, msg, msg_size, server_address);
      Sleep(1000);
    }
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}