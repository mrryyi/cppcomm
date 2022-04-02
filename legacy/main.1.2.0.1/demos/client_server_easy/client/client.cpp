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
  
  for (ever) {
    printf("Sending message with byte 0 containing 1\n");
    {
      // Note that message.buffer has no size yet.
      cppcomm::Message message;
      cppcomm::client_msg_one_write(message.buffer);
      auto ret = communicator.send_msg(message, server_address);
      if(ret != cppcomm::SUCCESS_SENDING_MESSAGE)
        printf("what the heck");
      Sleep(1000);
    }

    printf("Sending message with byte 0 containing 2\n");
    {
      cppcomm::Message message;
      // Note that message.buffer has no size yet.
      cppcomm::client_msg_two_write(message.buffer);
      // Now it does.
      auto ret = communicator.send_msg(message, server_address);
      if(ret != cppcomm::SUCCESS_SENDING_MESSAGE)
        printf("what the heck");
      Sleep(1000);
    }
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}