#include "../../../src/cppcomm.hpp"
#include "../common.hpp"

int main(int argc, char* argv[]) {

  printf("Program Started: %s\n", argv[0]); 

  const char * local_inet_address = "127.0.0.1";

  cppcomm::Communicator communicator;
  auto ret = communicator.init_nonblocking_udp(local_inet_address, PORT_CLIENT);
  if (ret != cppcomm::SUCCESS_INIT) {
    WSACleanup();
    return 0;
  }

  //----------------------------------
  // Create an address to send messages to.
  SOCKADDR_IN server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons( PORT_SERVER );
  server_address.sin_addr.S_un.S_addr = inet_addr(local_inet_address);
  
  printf("Running.\nSending messages...\n");

  cppcomm::Message ack_message;
  create_ack_message(ack_message, ACK_HELLO);
  
  cppcomm::Message ack_

  for (ever) {


    auto ret_send = communicator.send_msg(ack_message, server_address);
    if(ret_send != cppcomm::SUCCESS_SENDING_MESSAGE)
      printf("what the heck");

    auto ret_recv = communicator.recv_msg()
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}