#pragma once
#include "../../src/cppcomm.hpp"

#define ever ;;
constexpr static int PORT_SERVER = 6969;
constexpr static int PORT_CLIENT = 2020;

enum MSG_TYPE {
  ACK_HELLO = 1,
  ACK_HELLO_REPLY,
  MSG_STRING
};

void create_client_connect_request_message(cppcomm::Message & message) {
  message.buffer
};

void read_client_connect_request_message() {

};

void create_ack_message(cppcomm::Message & message, const MSG_TYPE& msg_type,  const int32& ACK_ID) {
  message.buffer.resize(2 * sizeof(uint32));
  message.buffer.write_uint32(msg_type);
  message.buffer.write_uint32(ACK_ID);
};

void read_ack_message(cppcomm::Message & message, MSG_TYPE& msg_type, int32& ACK_ID) {
  msg_type = read_message_type(message);
  ACK_ID = message.buffer.read_uint32();
};

void create_string_message(cppcomm::Message & message, const std::string& msg) {
  message.buffer.resize(sizeof(uint32) + msg.size());
  message.buffer.write_uint32(MSG_STRING);
  message.buffer.write_string(msg);
};

MSG_TYPE read_message_type(cppcomm::Message & message) {
  return (MSG_TYPE) message.buffer.read_int32();
};
