CXX := afl-clang-fast++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS := ../../libvgFuzz.so

TARGET := a.out
SRC := test.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean