#include "../src/cppcomm.hpp"

struct TEST_DATA {
    const bool8 success;
    const std::string function_that_tests;
    const std::string what_was_tested;
    const std::string what_should_happen;
};

TEST_DATA read_write_basic_uint8();
void print_test(TEST_DATA test_data);

int main (int args, const char* argv[]) {
        
    // What do we want to test?
    // The byte buffer read/write.
    // We can test the functionality, to start.
    // The point of writing to a buffer is so that it can be read and interpreted
    // So how about we just do expected behaviour first, and then we go into ways edge cases and ways to mess up?

    printf("Starting tests...\n");
    print_test(read_write_basic_uint8());

    return 0;
}

TEST_DATA read_write_basic_uint8() {
    const std::string function_that_tests = __func__;
    const std::string what_is_being_tested = "write_uint8, read_uint8";
    const std::string what_should_happen = "should write and read giving same values in order";

    using namespace cppcomm;
    // Prepare test-components
    // -----------------------
    uint8 buffer[3];
    uint8* write_iterator = buffer;
    // distinguishable values..
    uint8 w_byte0 = 0x32;
    uint8 w_byte1 = 0x61;
    uint8 w_byte2 = 0x90;
    
    write_uint8(&write_iterator, w_byte0);
    write_uint8(&write_iterator, w_byte1);
    write_uint8(&write_iterator, w_byte2);

    uint8* read_iterator = buffer;
    // Bytes to read data to.
    uint8 r_byte0;
    uint8 r_byte1;
    uint8 r_byte2;

    read_uint8(&read_iterator, &r_byte0);
    read_uint8(&read_iterator, &r_byte1);
    read_uint8(&read_iterator, &r_byte2);

    if (   r_byte0 == w_byte0
        && r_byte1 == w_byte1
        && r_byte2 == w_byte2

        // Negatory check as well for fun
        && r_byte0 != w_byte1
        && r_byte1 != w_byte2
        && r_byte2 != w_byte0
        )
        return TEST_DATA { true, function_that_tests, what_is_being_tested, what_should_happen};
    else
        return TEST_DATA { false, function_that_tests, what_is_being_tested, what_should_happen};
}

void print_test(TEST_DATA test_data) {
    printf("[%s] %s %s in %s()",
                      test_data.success ? "OK" : "XX",
                      test_data.what_was_tested.c_str(),
                      test_data.what_should_happen.c_str(),
                      test_data.function_that_tests.c_str());
}
