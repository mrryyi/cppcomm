#pragma once
#include <winsock2.h> // for udp
#include <winerror.h> // for getting WSAGetLastError() to do something useful.
#include <ws2tcpip.h> // for whatever
#include <iphlpapi.h> // for whatever
#include <iostream> // for printf
#include <stdio.h>
#include <stdlib.h>
#include <vector>

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
constexpr static int32_retval FAILURE_MESSAGE_TOO_LARGE_TO_SEND    = -505;
constexpr static int32_retval FAILURE_MESSAGE_HAS_NO_DATA          = -506;

// 4xx
constexpr static int32_retval FAILURE_SENDING_MESSAGE              = -400;
constexpr static int32_retval FAILURE_RECEIVING_MESSAGE            = -401;

// 2xx
constexpr static int32_retval SUCCESS_SENDING_MESSAGE   = 200;
constexpr static int32_retval SUCCESS_RECEIVING_MESSAGE = 201;
constexpr static int32_retval SUCCESS_INIT              = 202;

// https://stackoverflow.com/a/1098940
// Don't use this size. 
constexpr static uint16 MAX_UDP_PACKET_SIZE = 65507;

// https://stackoverflow.com/a/35697810
// Paraphrased from Beejor (2016-08-05 13:28):
// The question of packet size is not about "will my car get stuck in traffic"
// But more like "will my car fit under the bridge".
// Routers can legally put zero effort into handling big packets.
// Smaller packets are guaranteed to be handled as best they can by routers.
constexpr static uint16 MAX_SAFE_UDP_PACKET_SIZE = 508;


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

// Buffer class with uint8 vector as storage
// Needs to prevent buffer overflow
class ByteBuffer {
private:
  std::vector<uint8> m_buffer;
  size_t m_capacity;
  size_t m_write_pos;
  size_t m_read_pos;
public:
  ByteBuffer() : m_buffer(0), m_capacity(0), m_write_pos(0), m_read_pos(0) {}
  ByteBuffer(const size_t size) : m_buffer(size), m_capacity(size), m_write_pos(0), m_read_pos(0) {}
  ByteBuffer(const ByteBuffer& buffer) : m_buffer(buffer.m_buffer),
                                         m_capacity(buffer.m_capacity),
                                         m_write_pos(buffer.m_write_pos),
                                         m_read_pos(buffer.m_read_pos) {}
                                         
  ByteBuffer(ByteBuffer&& buffer) : m_buffer(std::move(buffer.m_buffer)),
                                    m_capacity(buffer.m_capacity),
                                    m_write_pos(buffer.m_write_pos),
                                    m_read_pos(buffer.m_read_pos) {}
  
  ByteBuffer& operator=(const ByteBuffer& buffer) {
    m_buffer = buffer.m_buffer;
    m_capacity = buffer.m_capacity;
    m_write_pos = buffer.m_write_pos;
    m_read_pos = buffer.m_read_pos;
    return *this;
  }
  
  ByteBuffer& operator=(ByteBuffer&& buffer) {
    m_buffer = std::move(buffer.m_buffer);
    m_capacity = buffer.m_capacity;
    m_write_pos = buffer.m_write_pos;
    m_read_pos = buffer.m_read_pos;
    return *this;
  }

  // For reading
  const auto buffer_read_pointer() { return m_buffer.data(); }
  // For writing
  auto buffer_write_pointer() { return m_buffer.data(); }

  void resize(const size_t size) {
    m_buffer.resize(size);
    m_read_pos = 0;
    m_write_pos = 0;
    m_capacity = size;
  }

  void erase_and_reset() {
    m_buffer.erase(m_buffer.begin(), m_buffer.end());
    write_all_to_zero();
    m_write_pos = 0;
    m_read_pos = 0;
  }

  void reset_read_pos() { m_read_pos = 0; }
  void reset_write_pos() { m_write_pos = 0; }
  
  const size_t capacity() const noexcept { return m_capacity; }
  const size_t size() const noexcept { return m_buffer.size(); }
  const size_t write_pos() const noexcept { return m_write_pos; }
  const size_t read_pos() const noexcept { return m_read_pos; }
  const size_t remaining() const noexcept { return m_capacity - m_write_pos; }
  const size_t available() const noexcept { return m_capacity - m_read_pos; }
  const bool empty() const noexcept { return m_write_pos == m_read_pos; }
  const bool full() const noexcept { return m_write_pos == m_capacity; }

  void write_all_to_zero() {
    for (size_t i = 0; i < m_capacity; i++) {
      m_buffer[i] = 0;
    }
  }

  // Writes data to buffer and increments the writing position
  void write_any_data(const void* data, const size_t& size) {
    if (m_write_pos + size > m_capacity)
      throw std::runtime_error("Buffer overflow");
    else {
      memcpy(&m_buffer[m_write_pos], data, size);
      m_write_pos += size;
    }
  }

  template<typename T>
  void write_singular_data(const T& data) {
    if (m_write_pos + sizeof(T) > m_capacity)
      throw std::runtime_error("Buffer overflow");
    else {
      m_buffer[m_write_pos] = data;
      m_write_pos += sizeof(T);
    }
  }

  void write_int8    (const int8& data)    { write_singular_data<int8>(data); }
  void write_int16   (const int16& data)   { write_singular_data<int16>(data); }
  void write_int32   (const int32& data)   { write_singular_data<int32>(data); }
  void write_int64   (const int64& data)   { write_singular_data<int64>(data); }
  void write_uint8   (const uint8& data)   { write_singular_data<uint8>(data); }
  void write_uint16  (const uint16& data)  { write_singular_data<uint16>(data); }
  void write_uint32  (const uint32& data)  { write_singular_data<uint32>(data); }
  void write_uint64  (const uint64& data)  { write_singular_data<uint64>(data); }
  void write_float32 (const float32& data) { write_singular_data<float32>(data); }
  void write_float64 (const float64& data) { write_singular_data<float64>(data); }
  void write_string  (const std::string& data) {
    write_uint32(data.size()); // Define string size for reading
    write_any_data(data.c_str(), data.size());
  }

  // Reads data from buffer and increments the reading position
  void read_any_data(void* data, const size_t& size) {
    if (m_read_pos + size > m_capacity)
      throw std::runtime_error("Buffer overflow");
    else {
      memcpy(data, &m_buffer[m_read_pos], size);
      m_read_pos += size;
    }
  }

  template<typename T>
  T read_singular_data() {
    if (m_read_pos + sizeof(T) > m_capacity)
      throw std::runtime_error("Buffer overflow");
    else {
      T data = m_buffer[m_read_pos];
      m_read_pos += sizeof(T);
      return data;
    }
  }

  int8    read_int8()    { return read_singular_data<int8>(); }
  int16   read_int16()   { return read_singular_data<int16>(); }
  int32   read_int32()   { return read_singular_data<int32>(); }
  int64   read_int64()   { return read_singular_data<int64>(); }
  uint8   read_uint8()   { return read_singular_data<uint8>(); }
  uint16  read_uint16()  { return read_singular_data<uint16>(); }
  uint32  read_uint32()  { return read_singular_data<uint32>(); }
  uint64  read_uint64()  { return read_singular_data<uint64>(); }
  float32 read_float32() { return read_singular_data<float32>(); }
  float64 read_float64() { return read_singular_data<float64>(); }
  void read_string(std::string& data) {
    uint32 size = read_uint32(); // Read string size that was written in this.write_string()
    data.resize(size);
    read_any_data(&data[0], size);
  }

};

struct Message {
  ByteBuffer buffer;
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
  // Returns SUCCESS_INIT if socket-making successful
  [[nodiscard]]
  auto init_nonblocking_udp(const char * ip, const uint16& port, const int32 socket_buffer_size = 2147483647) noexcept {
    
    if (m_initialized)
      return FAILURE_ALREADY_INITIALIZED;
    
    m_socket_buffer_size = socket_buffer_size;

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
  // Returns FAILURE_MESSAGE_TOO_LARGE_TO_SEND if message is too large to send
  // Returns FAILURE_SENDING_MESSAGE if sendto failed
  // Returns SUCCESS_SENDING_MESSAGE if success
  [[nodiscard]]
  auto send_msg(Message& s_Msg, const SOCKADDR_IN& address) noexcept {
    if (!m_initialized)
      return FAILURE_COMMUNICATOR_NOT_INITIALIZED;
    
    // .size() is just the size of what we wrote to it, not the whole capacity of the buffer.
    // If this is higher than the socket buffer size, we will not send all data indended in the message
    // Therefore we count it as a failure,
    // instead of only sending m_socket_buffer_size bytes out of the whole message we intended to send.
    if (s_Msg.buffer.size() > MAX_UDP_PACKET_SIZE || s_Msg.buffer.size() > m_socket_buffer_size)
      return FAILURE_MESSAGE_TOO_LARGE_TO_SEND;
    
    if (s_Msg.buffer.size() < 1)
      return FAILURE_MESSAGE_HAS_NO_DATA;

    s_Msg.bytes_handled = sendto(
                              m_socket, // through this socket
                (const char*) s_Msg.buffer.buffer_read_pointer(), // give the socket this buffer for reading
                              s_Msg.buffer.size(), // which has data for this many bytes
                              s_Msg.flags,        // with these flags
                  (SOCKADDR*) &address,           // to this address
                              s_Msg.address_size  // which is this long
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

    // Changes capacity of buffer to be the size of max UDP packet size.
    recv_msg.buffer.resize(MAX_UDP_PACKET_SIZE);
    recv_msg.buffer.erase_and_reset();

    recv_msg.bytes_handled = recvfrom(m_socket, // from this socket
                                  (char *) recv_msg.buffer.buffer_write_pointer(), // give the socket this buffer to write the message into
                                  m_socket_buffer_size,           // the socket can only receive these many bytes
                                  recv_msg.flags,                 // with these flags
                                  (SOCKADDR*)&recv_msg.address,   // from this address
                                  (int*) &recv_msg.address_size); // which is this long


    return (recv_msg.bytes_handled != SOCKET_ERROR)
          ? FAILURE_RECEIVING_MESSAGE
          : SUCCESS_RECEIVING_MESSAGE;
  }
  
  const auto socket_buffer_size() const noexcept {
    return m_socket_buffer_size;
  }

private:
  // Local management
  bool8         m_initialized = false;

  // Socket specific
  SOCKET        m_socket;
  SOCKADDR_IN   m_local_address;
  uint16        m_local_port;
  uint32        m_socket_buffer_size;

  bool8 make_nonblocking_udp_socket(SOCKET* out_socket) { 
    constexpr static int address_family = AF_INET;
    constexpr static int type = SOCK_DGRAM;
    constexpr static int protocol = IPPROTO_UDP;

    SOCKET sock;
    sock = socket(address_family, type, protocol);

    constexpr static int iOptLen = sizeof(m_socket_buffer_size); 

    // If no error occurs, setsockopt returns zero
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &m_socket_buffer_size, iOptLen)) {
      printf("make_nonblocking_udp_socket failed to set rcvbuf size: %d\n", WSAGetLastError());
      return false;
    }
    
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*) &m_socket_buffer_size, iOptLen)) {
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
    if (result == SOCKET_ERROR) {
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
  
// Below functions are purely for demo purposes.
void client_msg_one_write( ByteBuffer& buffer) {
  buffer.resize(sizeof(uint8));
  buffer.write_uint8(1);
};

void client_msg_two_write( ByteBuffer& buffer) {
  buffer.resize(sizeof(uint8));
  buffer.write_uint8(2);
};

void client_msg_1byte_read( ByteBuffer& buffer, uint8& ui8) {
  ui8 = buffer.read_uint8();
}

};// End namespace