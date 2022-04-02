CXX = g++ -static -static-libgcc -static-libstdc++ -std=c++2a
COMPILE_FLAGS = -c
LIBRARIES =
LDFLAGS = $(LIBRARIES) -lwsock32 -Wall -Wextra

TARGET = perf.exe
cpp_files = perf_ByteBuffer.cpp
objects = $(cpp_files:.cpp=.o)

all: $(TARGET)

$(TARGET): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY : clean
clean :
	-rm $(TARGET) $(objects) *.o