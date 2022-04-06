#include "../src/cppcomm.hpp"
#include <limits>
#include <string>

struct TEST_DATA {
    bool8 success;
    std::string function_that_tests;
    std::string what_was_tested;
    std::string what_should_happen;
    std::string failure_report;
};

TEST_DATA read_write_ByteBuffer_mixed_types();
TEST_DATA read_write_ByteBuffer_strings();
TEST_DATA read_write_ByteBuffer_any_data();
TEST_DATA reset_ByteBuffer();
TEST_DATA resize_to_fit_content_ByteBuffer();
TEST_DATA remaining_ByteBuffer();
TEST_DATA Communicator_initialize_socket_with_size(const char * ip, const uint16 port, const int32& size);

void print_test(TEST_DATA test_data);

int main (int args, const char* argv[]) {
        
    // What do we want to test?
    // The byte buffer read/write.
    // We can test the functionality, to start.
    // The point of writing to a buffer is so that it can be read and interpreted
    // So how about we just do expected behaviour first, and then we go into ways edge cases and ways to mess up?

    const char * local_ip = "127.0.0.1";

    uint16 port = 42069;
    const uint32 max_int32 = 2147483647;
    const uint32 oneGB = 1073741824;
    const uint32 halfGB = 536870912;
    const uint32 quarterGB = 268435456;
    const uint32 oneMB = 1048576;

    printf("Starting tests...\n");
    print_test(read_write_ByteBuffer_any_data());
    print_test(read_write_ByteBuffer_strings());
    print_test(read_write_ByteBuffer_mixed_types());
    print_test(reset_ByteBuffer());
    print_test(resize_to_fit_content_ByteBuffer());
    print_test(remaining_ByteBuffer());
    print_test(Communicator_initialize_socket_with_size(local_ip, port++, max_int32));
    print_test(Communicator_initialize_socket_with_size(local_ip, port++, oneGB));
    print_test(Communicator_initialize_socket_with_size(local_ip, port++, halfGB));
    print_test(Communicator_initialize_socket_with_size(local_ip, port++, quarterGB));
    print_test(Communicator_initialize_socket_with_size(local_ip, port++, oneMB));

    printf("Finished tests.\n");

    return 0;
}


TEST_DATA Communicator_initialize_socket_with_size(const char * ip, const uint16 port, const int32& size) {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "Communicator::init_nonblocking_udp()";
    test_data.what_should_happen = "should be able to initialize a socket with size " + std::to_string(size) + " bytes";
    test_data.failure_report = "";

    cppcomm::Communicator communicator;
    auto ret = communicator.init_nonblocking_udp(ip, port, size);
    if (ret != cppcomm::SUCCESS_INIT) {
        test_data.success = false;
        test_data.failure_report = "Initialized socket with " + std::to_string(size) +  " bytes, which is too big.";
    }
    
    return test_data;
}

TEST_DATA remaining_ByteBuffer() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "ByteBuffer::remaining()";
    test_data.what_should_happen = "should return the number of bytes remaining in the buffer";
    test_data.failure_report = "";

    cppcomm::ByteBuffer buffer;

    // decide size
    const uint32 size = 1024;

    // Size the buffer
    buffer.resize(size);

    // Write some data
    buffer.write_int32(1);
    buffer.write_int32(2);
    buffer.write_int32(3);
    buffer.write_int32(4);

    // Check the remaining size
    if (buffer.remaining() != size - 4 * sizeof(int32)) {
        test_data.success = false;
        test_data.failure_report = "Remaining size is not " + std::to_string(size - 4 * sizeof(int32)) + " bytes";
    }

    return test_data;
}

TEST_DATA resize_to_fit_content_ByteBuffer() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "ByteBuffer::reset_to_fit_content()";
    test_data.what_should_happen = "should make size and capacity equal, and not tamper with the data";
    test_data.failure_report = "";

    cppcomm::ByteBuffer buffer;
    // Four too many bytes. 
    buffer.resize(5 * sizeof(int32));
    buffer.write_int32(1);
    buffer.write_int32(2);
    buffer.write_int32(3);
    buffer.write_int32(4);

    buffer.resize_to_fit_content();

    if (buffer.capacity() != buffer.write_pos()) {
        test_data.success = false;
        test_data.failure_report = "Size not equal to capacity.";
    }

    auto one = buffer.read_int32();
    auto two = buffer.read_int32();
    auto three = buffer.read_int32();
    auto four = buffer.read_int32();

    if (  one != 1
       || two != 2
       || three != 3
       || four != 4 )
    {
        test_data.success = false;
        test_data.failure_report.append("Resizing to fill altered data.");

    }

    return test_data;
}

// Should write data to the buffer, reset it, and have it be empty.
TEST_DATA reset_ByteBuffer() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "ByteBuffer::reset()";
    test_data.what_should_happen = "should be able to reset ByteBuffer";
    test_data.failure_report = "";

    cppcomm::ByteBuffer buffer;
    buffer.resize(4 * sizeof(int32));
    buffer.write_int32(1);
    buffer.write_int32(2);
    buffer.write_int32(3);
    buffer.write_int32(4);

    buffer.reset();

    auto zero1 = buffer.read_int32();
    auto zero2 = buffer.read_int32();
    auto zero3 = buffer.read_int32();
    auto zero4 = buffer.read_int32();

    // All zerox values should be zero
    if (zero1 != 0 || zero2 != 0 || zero3 != 0 || zero4 != 0) {
        test_data.success = false;
        test_data.failure_report = "ByteBuffer was not reset correctly.";
    }

    return test_data;
}

TEST_DATA read_write_ByteBuffer_mixed_types() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "Bytebuffer";
    test_data.what_should_happen = "should be able to read and write mixed types";
    test_data.failure_report = "";

    // Create a buffer
    cppcomm::ByteBuffer buffer(1024);

    // Write some data to the buffer
    buffer.write_int8(1);
    buffer.write_int16(2);
    buffer.write_int32(3);
    buffer.write_int64(4);
    buffer.write_uint8(1);
    buffer.write_uint16(2);
    buffer.write_uint32(3);
    buffer.write_uint64(4);
    buffer.write_float32(5.0f);
    buffer.write_float64(6.0);
    
    buffer.write_string("Hello World");

    // To make sure string is written correctly by having stuff after it.
    buffer.write_float64(7.0);

    // Read the data back
    int8_t int8_value   = buffer.read_int8();
    int16_t int16_value = buffer.read_int16();
    int32_t int32_value = buffer.read_int32();
    int64_t int64_value = buffer.read_int64();
    uint8_t uint8_value = buffer.read_uint8();
    uint16_t uint16_value = buffer.read_uint16();
    uint32_t uint32_value = buffer.read_uint32();
    uint64_t uint64_value = buffer.read_uint64();
    float float32_value = buffer.read_float32();
    double float64_value = buffer.read_float64();
    std::string string_value; buffer.read_string(string_value);
    double float64_value2 = buffer.read_float64();

    // Check the values
    if (   int8_value != 1 
        && int16_value != 2
        && int32_value != 3
        && int64_value != 4
        && uint8_value != 1 
        && uint16_value != 2
        && uint32_value != 3
        && uint64_value != 4
        && float32_value != 5.0f
        && float64_value != 6.0
        && string_value != "Hello World"
        && float64_value2 != 7.0)
    {
        test_data.success = false;
        test_data.failure_report = "Failed to read back the data correctly";
    }
    
    return test_data;
}

TEST_DATA read_write_ByteBuffer_strings() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "ByteBuffer::write_string() and ByteBuffer::read_string()";
    test_data.what_should_happen = "should be able to write and read a string correctly";
    test_data.failure_report = "";

    // Create a ByteBuffer
    cppcomm::ByteBuffer buffer(1024);

    // Write a string to the buffer
    buffer.write_string("Hello World");

    // Read the string back from the buffer
    std::string read_string;
    buffer.read_string(read_string);

    // Check that the string was read correctly
    if (read_string != "Hello World") {
        test_data.success = false;
        test_data.failure_report = "The string was not read correctly";
    }

    return test_data;
}

TEST_DATA read_write_ByteBuffer_any_data(){
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "ByteBuffer::write_any_data()";
    test_data.what_should_happen = "should write and read back the same data";
    test_data.failure_report = "";

    // Create a ByteBuffer
    cppcomm::ByteBuffer buffer(1024);

    // Write some random data to it
    uint8_t random_data[1024];
    for(int i = 0; i < 1024; i++){ random_data[i] = rand() % 255; }
    buffer.write_any_data(random_data, 1024);

    // Read the data back
    uint8_t read_data[1024];
    buffer.read_any_data(read_data, 1024);

    // Compare the data
    for(int i = 0; i < 1024; i++){
        if(random_data[i] != read_data[i]){
            test_data.success = false;
            test_data.failure_report = "The data was not read back correctly";
            break;
        }
    }

    return test_data;
}

void print_test(TEST_DATA test_data) {
    printf("[%s] %s %s in %s()",
                      test_data.success ? "OK" : "XX",
                      test_data.what_was_tested.c_str(),
                      test_data.what_should_happen.c_str(),
                      test_data.function_that_tests.c_str());

    if (!test_data.success) {
        printf("\n\t failure report: %s", test_data.failure_report.c_str());
    }

    printf("\n");
}
