#include "../../src/cppcomm.hpp"
#include <chrono>
#include <functional>

// From main.1.1.0.0
// writes uint8, increments iterator.
static void write_uint8(uint8** buffer_iterator, const uint8& ui8) {
  // Set the value at reader iterator
  **buffer_iterator = ui8;
  // Increase the iterator
  ++(*buffer_iterator);
}

void test_ByteBuffer_fill_1024_sized_buffer_with_uint8_resize() {
  cppcomm::ByteBuffer buffer;
  buffer.resize(1024);
  for (uint64 i = 0; i < buffer.capacity() / sizeof(uint8) /* lol */; ++i) {
    buffer.write_uint8(i % 256);
  }
}

void test_ByteBuffer_fill_1024_sized_buffer_with_uint8_no_resize() {
  cppcomm::ByteBuffer buffer(1024);
  for (uint64 i = 0; i < buffer.capacity() / sizeof(uint8) /* lol */; ++i) {
    buffer.write_uint8(i % 256);
  }
}

void test_RawPointers_fill_1024_sized_buffer_with_uint8() {

  // This is how it would have been 
  uint8 buffer[1024];
  uint8* buffer_iterator = buffer;
  
  for (int i = 0; i < 1024 / sizeof(uint8) /* lol */; ++i) {
    write_uint8(&buffer_iterator, i % 256);
  }
}


static void write_uint32(uint8** buffer_iterator, uint32 i32) {
    memcpy(*buffer_iterator, &i32, sizeof( i32 ));
    *buffer_iterator += sizeof( i32 );
};

void test_ByteBuffer_fill_4096_sized_buffer_with_uint32_resize() {
  cppcomm::ByteBuffer buffer;
  buffer.resize(4096);
  for (size_t i = 0; i < buffer.capacity() / sizeof(uint32); ++i) {
    buffer.write_uint32(i % 256);
  }
}

void test_ByteBuffer_fill_4096_sized_buffer_with_uint32_no_resize() {
  cppcomm::ByteBuffer buffer(4096);
  for (size_t i = 0; i < buffer.capacity() / sizeof(uint32); ++i) {
    buffer.write_uint32(i % 256);
  }
}

void test_RawPointers_fill_4096_sized_buffer_with_uint32() {
  
    // This is how it would have been 
    uint8 buffer[4096];
    uint8* buffer_iterator = buffer;
    
    for (int i = 0; i < 4096 / sizeof(uint32); ++i) {
      write_uint32(&buffer_iterator, i % 256);
    }
}


// Should contain iterations and time in microseconds
struct PerformanceTestData {
  uint64 iterations;
  std::chrono::microseconds time_spent;
};

PerformanceTestData test_function_time(uint64 iterations,
                                       std::function<void()> func) {
  auto start = std::chrono::high_resolution_clock::now();
  for (uint64 i = 0; i < iterations; i++) {
    func();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time_spent =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return {iterations, time_spent};
}

struct AvgPerformanceTestData {
  uint64 iterations_for_avg;
  uint64 iterations_for_func;
  std::chrono::microseconds avg_time_spent {0};
};

// Returns average time spent per test_function_time result
AvgPerformanceTestData test_function_time_avg(uint64 iterations_for_avg,
                                              uint64 iterations_for_func,
                                               std::function<void()> func) {
  AvgPerformanceTestData avg_data;
  avg_data.iterations_for_avg = iterations_for_avg;
  avg_data.iterations_for_func = iterations_for_func;

  for (uint64 i = 0; i < iterations_for_avg; i++) {
    auto data = test_function_time(iterations_for_func, func);
    avg_data.avg_time_spent += data.time_spent;
  }
  avg_data.avg_time_spent /= iterations_for_avg;
  return avg_data;
}

void print_PerformanceTestData(const PerformanceTestData& data) {
  std::cout << "Iterations: " << data.iterations << '\n';
  std::cout << "Time spent: " << data.time_spent.count() << " microseconds" << '\n';
}

void print_AvgPerformanceTestData(const AvgPerformanceTestData& data) {
  std::cout << "Iterations for avg: " << data.iterations_for_avg << '\n';
  std::cout << "Iterations for func: " << data.iterations_for_func << '\n';
  std::cout << "Avg time spent: " << data.avg_time_spent.count() << " microseconds" << '\n';
}

int main (int args, const char* argv[]) {

  uint64 iterations_for_avg = 100;
  uint64 iterations_for_func = 1000;

  bool test_all = args > 1 ? false : true;

  if (test_all || argv[1] == std::string("bytefill")) {
    // Safe
    std::cout << "test_ByteBuffer_fill_1024_sized_buffer_with_uint8_resize" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_ByteBuffer_fill_1024_sized_buffer_with_uint8_resize));
    // Safe
    std::cout << "test_ByteBuffer_fill_1024_sized_buffer_with_uint8_no_resize" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_ByteBuffer_fill_1024_sized_buffer_with_uint8_no_resize));
    // Non-safe variant
    std::cout << "test_RawPointers_fill_1024_sized_buffer_with_uint8" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_RawPointers_fill_1024_sized_buffer_with_uint8));
  }

  if (test_all || argv[1] == std::string("4bytefill")) {
    // Safe
    std::cout << "test_ByteBuffer_fill_4096_sized_buffer_with_uint32_resize" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_ByteBuffer_fill_4096_sized_buffer_with_uint32_resize));
    // Safe
    std::cout << "test_ByteBuffer_fill_4096_sized_buffer_with_uint32_no_resize" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_ByteBuffer_fill_4096_sized_buffer_with_uint32_no_resize));
    // Non-safe variant
    std::cout << "test_RawPointers_fill_4096_sized_buffer_with_uint32" << '\n';
    print_AvgPerformanceTestData(test_function_time_avg(iterations_for_avg, iterations_for_func, test_RawPointers_fill_4096_sized_buffer_with_uint32));
  }

  return 0;
}