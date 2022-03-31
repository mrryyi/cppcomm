#include "../../../src/cppcomm.hpp"
#include "../common.hpp"

int main(int argc, char* argv[]) {

  printf("Program Started: %s\n", argv[0]); 

  const char * local_inet_address = "127.0.0.1";

  cppcomm::Communicator communicator;
  auto ret = communicator.init_nonblocking_udp(local_inet_address, PORT_SERVER);
  if (ret != cppcomm::SUCCESS_INIT) {
    WSACleanup();
    return 0;
  }
  
  for (ever) {
    // Receive messages.
    cppcomm::Message msg;
    auto ret = communicator.recv_msg(msg);
    
    if (msg.bytes_handled != SOCKET_ERROR) {
      printf("Received message.\n");
      uint8 byte_for_reading;
      cppcomm::client_msg_1byte_read(msg.buffer, &byte_for_reading);

      switch(byte_for_reading) {
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
    
    Sleep(1000);
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}