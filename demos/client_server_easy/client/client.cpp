#include "../../../src/cppcomm.hpp"
#include "../common.hpp"

void client_msg_one_write( cppcomm::ByteBuffer& buffer) {
  buffer.set_capacity(sizeof(uint8));
  buffer.write_uint8(1);
};

void client_msg_two_write( cppcomm::ByteBuffer& buffer) {
  buffer.set_capacity(sizeof(uint8));
  buffer.write_uint8(2);
};

int main(int argc, char* argv[]) {

  printf("Program Started: %s\n", argv[0]); 

  const char * local_inet_address = "127.0.0.1";

  cppcomm::Communicator communicator;
  auto ret_init = communicator.init_nonblocking_udp(local_inet_address, PORT_CLIENT);
  if (ret_init != cppcomm::SUCCESS_INIT) {
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

  cppcomm::Message msg_first_byte_one;
  // Note that message.buffer has no size yet.
  client_msg_one_write(msg_first_byte_one.buffer);
  // Now it does, changed in the function.

  cppcomm::Message msg_first_byte_two;
  client_msg_two_write(msg_first_byte_two.buffer);

  bool every_other_flipper = true;

  for (ever) {
    
    // Send one or two every other loop
    if (every_other_flipper) {
      printf("Sending one\n");
      auto ret = communicator.send_msg( msg_first_byte_one, server_address);
      if(ret != cppcomm::SUCCESS_SENDING_MESSAGE)
        printf("what the heck");
    }
    else {
      printf("Sending two\n");
      auto ret = communicator.send_msg( msg_first_byte_two, server_address);
      if(ret != cppcomm::SUCCESS_SENDING_MESSAGE)
        printf("what the heck");
    }
    
    Sleep(1000);

    every_other_flipper = !every_other_flipper;
  }
  
  WSACleanup();
  exit(EXIT_SUCCESS);
}