#pragma once
#include <winsock2.h> // for udp
#include <winerror.h> // for getting WSAGetLastError() to do something useful.
#include <ws2tcpip.h> // for whatever
#include <iphlpapi.h> // for whatever
#include <iostream> // for printf
#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Saves time being explicit not having to google sizes.
// I know, your subconscious says "Pshh, he has to google that?"
// I guess not everyone can be like you, my liege.
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef int32 bool32;
typedef uint8 bool8;
typedef float float32;
typedef double float64;
typedef int int32_retval;

namespace cppcomm
{
// STATUS CODES
// -5xx = meta/infra-specific error codes.
// -4xx = functionality-specific error codes.
// 2xx = successful codes.

// 5xx
constexpr static int32_retval FAILURE_COMMUNICATOR_NOT_INITIALIZED = -500;
constexpr static int32_retval FAILURE_INITIALIZING_WINSOCK         = -501;
constexpr static int32_retval FAILURE_MAKE_SOCKET                  = -502;
constexpr static int32_retval FAILURE_BIND_SOCKET                  = -503;
constexpr static int32_retval FAILURE_ALREADY_INITIALIZED          = -504;

// 4xx
constexpr static int32_retval FAILURE_SENDING_MESSAGE              = -400;
constexpr static int32_retval FAILURE_RECEIVING_MESSAGE            = -400;

// 2xx
constexpr static int32_retval SUCCESS_SENDING_MESSAGE   = 200;
constexpr static int32_retval SUCCESS_RECEIVING_MESSAGE = 201;
constexpr static int32_retval SUCCESS_INIT              = 202;

constexpr static int32 SOCKET_BUFFER_SIZE = 1024;

const static void print_address(const SOCKADDR_IN& address){
  //, const std::ostream& os = std::cout) { // implement this at a later point
  // This is fine for now
  printf( "%d.%d.%d.%d:%d", 
  address.sin_addr.S_un.S_un_b.s_b1, 
  address.sin_addr.S_un.S_un_b.s_b2, 
  address.sin_addr.S_un.S_un_b.s_b3, 
  address.sin_addr.S_un.S_un_b.s_b4, 
  address.sin_port);
}; // end print_address

struct Message {
  uint8 buffer[SOCKET_BUFFER_SIZE];
  uint32 SOCKADDR_IN_size;
  uint32 flags = 0;
  SOCKADDR_IN address;
  uint32 address_size = sizeof(Message::address);
  uint32 bytes_handled = SOCKET_ERROR; // Bytes that have been received/sent

  void set_address(const SOCKADDR_IN& address ) noexcept {
    this->address = address;
  } // end set_address

};// End Message struct

// Returns true if winsock has been, at one point, successfully initialized.
// Returns false if attempt to initialize winsock is unsuccessful.
const static bool8 init_winsock() noexcept {
  // We need only one successful startup of WSA.
  static bool initialized = false; 

  if (!initialized) {
    int     iResult;
    WSADATA wsaData; // Not used but could be.
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d", iResult);
        return false;
    }
  }

  initialized = true;
  return initialized;
}

// Does: manage socket, port, IP, message sending.
// Limited to one socket, port, etc.
// If you want more, open more.
// TODO: Get port and IP bound to the socket. Perhpas just return m_local_address.
class Communicator {
public:
  Communicator() noexcept {};

  // Initializes communication, opens and binds a socket to ip and port.
  // Returns FAILURE_ALREADY_INITIALIZED if Communication already has been initialized.
  // Returns FAILURE_INITIALIZING_WINSOCK if winsock initialization unsuccessful 
  // Returns FAILURE_MAKE_SOCKET if socket-making unsuccessful
  [[nodiscard]]
  auto init_nonblocking_udp(const char * ip, const uint16& port) noexcept {
    
    if (m_initialized)
      return FAILURE_ALREADY_INITIALIZED;

    //----------------------------------------
    // Initialize Winsock. Note: function does that only once per runtime.
    if (!init_winsock()) {
      printf("winsock failed: %d\n", WSAGetLastError() );
      return FAILURE_INITIALIZING_WINSOCK;
    }

    //----------------------------------------
    // Make a socket
    if (!make_nonblocking_udp_socket(&m_socket)) {
      printf("socket failed: %d\n", WSAGetLastError() );
      return FAILURE_MAKE_SOCKET;
    }

    //----------------------------------------
    // Make local address
    SOCKADDR_IN m_local_address;
    m_local_address.sin_family = AF_INET;
    m_local_address.sin_port = htons( port );
    m_local_address.sin_addr.s_addr = INADDR_ANY;

    //----------------------------------------
    // Bind the socket to local address and port.
    if( bind( m_socket, (SOCKADDR*)&m_local_address, sizeof( m_local_address ) ) == SOCKET_ERROR ) {
      printf( "bind failed: %d\n", WSAGetLastError() );
      return FAILURE_BIND_SOCKET;
    }
    
    m_initialized = true;
    return SUCCESS_INIT;
  }

  // Returns COMMUNICATOR_NOT_INITIALIZED if communicator not initialized
  // Returns FAILURE_SENDING_MESSAGE if sendto failed
  // Returns SUCCESS_SENDING_MESSAGE if success
  [[nodiscard]]
  auto send_msg(Message& s_Msg, const uint32& buffer_length, const SOCKADDR_IN& address) noexcept {
    if (!m_initialized)
      return FAILURE_COMMUNICATOR_NOT_INITIALIZED;

    s_Msg.bytes_handled = sendto(
                              m_socket,         // through this socket
                (const char*) s_Msg.buffer,      // send this block
                              buffer_length,     // which is this long
                              s_Msg.flags,       // with these flags
                  (SOCKADDR*) &address,          // to this address
                              s_Msg.address_size // which is this long
              );
    
    if (s_Msg.bytes_handled == SOCKET_ERROR) {
      printf( "sendto failed: %d", WSAGetLastError() );
      return FAILURE_SENDING_MESSAGE;
    }
    
    return SUCCESS_SENDING_MESSAGE;
  }

  // Returns COMMUNICATOR_NOT_INITIALIZED if communicator not initialized
  // Returns FAILURE_RECEIVING_MESSAGE if recvmsg failed
  // Returns SUCCESS_RECEIVING_MESSAGE if recvmsg succeeded
  [[nodiscard]]
  auto recv_msg(Message& recv_msg) noexcept {
    if (!m_initialized)
      return FAILURE_COMMUNICATOR_NOT_INITIALIZED;
    
    recv_msg.bytes_handled = recvfrom(m_socket,
                                  (char *) recv_msg.buffer,
                                  SOCKET_BUFFER_SIZE,
                                  recv_msg.flags,
                                  (SOCKADDR*)&recv_msg.address,
                                  (int*) &recv_msg.address_size);


    return (recv_msg.bytes_handled != SOCKET_ERROR)
          ? FAILURE_RECEIVING_MESSAGE
          : SUCCESS_RECEIVING_MESSAGE;
  }
  

private:
  bool8 m_initialized = false;

  SOCKET m_socket;
  SOCKADDR_IN m_local_address;
  int32 m_local_port;

  bool8 make_nonblocking_udp_socket(SOCKET* out_socket) { 
    constexpr static int address_family = AF_INET;
    constexpr static int type = SOCK_DGRAM;
    constexpr static int protocol = IPPROTO_UDP;

    SOCKET sock;
    sock = socket(address_family, type, protocol);

    constexpr static int iOptLen = sizeof(SOCKET_BUFFER_SIZE); 

    // If no error occurs, setsockopt returns zero
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &SOCKET_BUFFER_SIZE, iOptLen)) {
      printf("make_nonblocking_udp_socket failed to set rcvbuf size: %d\n", WSAGetLastError());
      return false;
    }
    
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*) &SOCKET_BUFFER_SIZE, iOptLen)) {
      printf("make_nonblocking_udp_socket failed to set sndbuf size: %d\n", WSAGetLastError());
      return false;
    }

    if (sock == INVALID_SOCKET) {
      printf("make_nonblocking_udp_socket failed: %d\n", WSAGetLastError());
      return false;
    }

    // 0x8004667E is the setting to make a socket non-blocking (FIONBIO doesnt want to define itself)
    // https://stackoverflow.com/a/16185001
    // :) 

    // put socket in non-blocking mode
    ULONG enabled = 1;
    int result = ioctlsocket(sock, 0x8004667E, &enabled);
    if (result == SOCKET_ERROR)
    {
      printf("ioctlsocket() failed: %d\n", WSAGetLastError());
      return false;
    }

    // Only if everything went fine do we set the parameter socket to the new socket.
    *out_socket = sock;

    return true;
  };

  // valid_digit and is_valid_ip gotten from:
  // https://www.includehelp.com/c-programs/check-string-is-valid-ipv4-address-or-not.aspx

  /* return 1 if string contain only digits, else return 0 */
  int valid_digit(char *ip_str) {
    while (*ip_str) {
      if (*ip_str >= '0' && *ip_str <= '9')
        ++ip_str;
      else
        return 0;
    }

    return 1;
  }

    /* return 1 if IP string is valid, else return 0 */
  int is_valid_ip(char *ip_str) {
    int i, num, dots = 0;
    char *ptr;

    if (ip_str == NULL)
      return 0;

    const char * DELIM = ".";

    ptr = strtok(ip_str, DELIM);

    if (ptr == NULL)
      return 0;

    while (ptr) {

      /* after parsing string, it must contain only digits */
      if (!valid_digit(ptr))
        return 0;

      num = atoi(ptr);

      /* check for valid IP */
      if (num >= 0 && num <= 255) {
        /* parse remaining string */
        ptr = strtok(NULL, DELIM);
        if (ptr != NULL)
            ++dots;
      } else
        return 0;

    }

    /* valid IP string must contain 3 dots */
    if (dots != 3)
      return 0;
    
    return 1;
  }
}; // End Communicator

  
// writes uint8, increments iterator.
static void write_uint8(uint8** buffer_iterator, const uint8& ui8) {
  **buffer_iterator = ui8;
  ++(*buffer_iterator);
}

// reads uint8, increments iterator.
static void read_uint8(uint8** buffer_iterator, uint8* ui8) {
  *ui8 = **buffer_iterator;
  ++(*buffer_iterator);
}

uint32 client_msg_one_write( uint8* buffer) {
  uint8* iterator = buffer;

  const static uint8 one = 1;

  write_uint8(&iterator, one);

  return (uint32)(iterator - buffer);
};

uint32 client_msg_two_write( uint8* buffer) {
    uint8* iterator = buffer;

    const static uint8 two = 2;

    write_uint8(&iterator, two);

    return (uint32)(iterator - buffer);
};

void client_msg_1byte_read(uint8* buffer, uint8* byte) {
  uint8* iterator = buffer;
  read_uint8(&iterator, byte);
}



};// End namespace