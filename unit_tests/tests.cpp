#include "../src/cppcomm.hpp"
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
void print_test(TEST_DATA test_data);

int main (int args, const char* argv[]) {
        
    // What do we want to test?
    // The byte buffer read/write.
    // We can test the functionality, to start.
    // The point of writing to a buffer is so that it can be read and interpreted
    // So how about we just do expected behaviour first, and then we go into ways edge cases and ways to mess up?

    printf("Starting tests...\n");
    print_test(read_write_ByteBuffer_any_data());
    print_test(read_write_ByteBuffer_strings());
    print_test(read_write_ByteBuffer_mixed_types());
    printf("Finished tests.\n");

    return 0;
}

TEST_DATA read_write_ByteBuffer_mixed_types() {
    TEST_DATA test_data;
    test_data.success = true;
    test_data.function_that_tests = __func__;
    test_data.what_was_tested = "Bytebuffer should be able to read and write mixed types";
    test_data.what_should_happen = "";
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
        && string_value != "Hello World")
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
    test_data.what_should_happen = "The ByteBuffer should be able to write and read a string correctly";
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
