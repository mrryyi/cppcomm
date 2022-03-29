#pragma once
#include <winsock2.h> // for udp
#include <ws2tcpip.h> // for whatever
#include <iphlpapi.h> // for whatever
#include <iostream> // for printf
#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Saves time being explicit not having to google sizes.
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

namespace cppcomm
{

constexpr int32 SOCKET_BUFFER_SIZE = 1024;

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
  uint32 bytes_received = SOCKET_ERROR;

  void set_address(const SOCKADDR_IN address ) {
    this->address = address;
  } // end set_address

};// End Message struct

// writes uint8, increments iterator.
static void  write_uint8(uint8** buffer, const uint8& ui8) {
  // Set the value at reader iterator
  **buffer = ui8;
  // Increase the iterator
  ++(*buffer);
}

static void read_uint8(uint8** buffer, uint8* ui8) {
  *ui8 = **buffer;
  ++(*buffer);
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

void send_msg(SOCKET* sock, Message& s_Msg, uint32 buffer_length, SOCKADDR_IN& address) {
    if (sendto( *sock,
                (const char*) s_Msg.buffer,
                buffer_length,
                s_Msg.flags,
                (SOCKADDR*)&address,
                sizeof( address )) == SOCKET_ERROR) {
        printf( "sendto failed: %d", WSAGetLastError() );
    }
}

bool8 make_socket(SOCKET* out_socket)
{
  int address_family = AF_INET;
  int type = SOCK_DGRAM;
  int protocol = IPPROTO_UDP;
  SOCKET sock;
  sock = socket(address_family, type, protocol);

  const static int SOCKET_BUFFER_SIZE = 1024;
  const static int iOptLen = sizeof(int); 

  if (!setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &SOCKET_BUFFER_SIZE, iOptLen))
  {
    printf("failed to set rcvbuf size: %d\n", WSAGetLastError());
  }
  if (!setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*) &SOCKET_BUFFER_SIZE, iOptLen))
  {
    printf("failed to set sndbuf size: %d\n", WSAGetLastError());
  }

  if (sock == INVALID_SOCKET)
  {
    printf("socket() failed: %d\n", WSAGetLastError());
    return false;
  }

  // put socket in non-blocking mode
  ULONG enabled = 1;

    // You may be wondering why we pass 0x8004667E as the second argument.
    // We actually want to pass FIONBIO, but it doesn't work, so we're using the value FIONBIO
    // would have if it worked. This results in the correct behaviour.
    // This issue took a million years to solve, including giving up on non-blocking sockets,
    // trying to settle for a billion threads to solve concurrency requirements,
    // encountering a quadrillion segfaults and WSA errors, refactoring here and there to accomodate
    // threading, then giving up to find this:
    //
    // https://stackoverflow.com/a/16185001
    //
    // :)

  int result = ioctlsocket(sock, 0x8004667E, &enabled);
  if (result == SOCKET_ERROR)
  {
    printf("ioctlsocket() failed: %d\n", WSAGetLastError());
    return false;
  }

    *out_socket = sock;

  return true;
};


// Below code stolen from:
// https://www.includehelp.com/c-programs/check-string-is-valid-ipv4-address-or-not.aspx

/* return 1 if string contain only digits, else return 0 */
int valid_digit(char *ip_str)
{
    while (*ip_str) {
        if (*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
            return 0;
    }
    return 1;
}

/* return 1 if IP string is valid, else return 0 */
int is_valid_ip(char *ip_str)
{
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

};// End namespace