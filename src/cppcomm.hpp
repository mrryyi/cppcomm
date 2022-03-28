#pragma once
#include <winsock2.h> // for udp
#include <ws2tcpip.h> // for whatever
#include <iphlpapi.h> // for whatever
#include <iostream> // for printf

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
  uint32 address_size;
  uint32 bytes_received = SOCKET_ERROR;

  void set_address(const SOCKADDR_IN address ) {
    this->address = address;
  } // end set_address

};// End Message struct

const static void send_message() {

}

// 
const static Message create_message() {
  Message m;
  return m;
}

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