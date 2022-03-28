#include "src/cppcomm.hpp"
#define ever ;;

int main(int argc, char* argv[]) {

  using namespace cppcomm;

  std::string inet_address = "127.0.0.1";

  // Here you can connect to different IPs than the default local.
  if( argc >= 2 ) {

    if ( is_valid_ip( argv[1] )) {
      inet_address = argv[1];
    }
    else {
      printf("Not valid IP: %s\n", inet_address.c_str() );
    }
  }

  // We create a WSADATA object called wsaData.
  WSADATA wsaData;

  // We call WSAStartup and return its value as an integer and check for errors.
  int iResult;
  // WSAStartup definition:
  // WSAStartup(
  // _In_ WORD wVersionRequested,
  // _Out_ LPWSADATA lpWSAData
  // );
  // MAKEWORD(2, 2) requests version 2.2 ow winsock.
  // WSAstartup makes a request for version 2.2 of Winsock on the system,
  // and sets the passed version as the highest version of Windows Sockets support
  // that the caller can use.
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      printf("WSAStartup failed: %d", iResult);
      exit(EXIT_FAILURE);
  }

  int address_family = AF_INET;
  int type = SOCK_DGRAM;
  int protocol = IPPROTO_UDP;

  SOCKET sock;

  if( !make_socket( &sock ) )
  {
    printf( "socket failed: %d", WSAGetLastError() );
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  srand( (unsigned) time(NULL) );
  u_short random_port = 1000 + rand() % 56000;

  SOCKADDR_IN local_address;
  local_address.sin_family = AF_INET;
  local_address.sin_port = random_port;//htons( PORT_HERE );
  local_address.sin_addr.s_addr = INADDR_ANY;

  if( bind( sock, (SOCKADDR*)&local_address, sizeof( local_address ) ) == SOCKET_ERROR )
  {
      printf( "bind failed: %d", WSAGetLastError() );
      exit(EXIT_FAILURE);
  }

  const int PORT_SERVER = 1234;

  SOCKADDR_IN server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons( PORT_SERVER );
  server_address.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1"/*inet_address.c_str()*/ );


  for(ever) {
    Message msg;
    msg.address = "127.0.0.1";
    send_msg(msg);
    Sleep(1000);
  }

  return 0;
}