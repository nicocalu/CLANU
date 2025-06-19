CXX = g++
CXXFLAGS = -Ilibmlp -Ilibread -Wall -Wextra -Ofast -std=c++20

# Source and header files
LIBMLP_SRCS := $(wildcard libmlp/*.cpp)
LIBMLP_HDRS := $(wildcard libmlp/*.h)
LIBREAD_SRCS := $(wildcard libread/*.cpp)
LIBREAD_HDRS := $(wildcard libread/*.h)
TEST_SRC := irm/irm_test/irm_mlp_test.cpp

OBJS := $(LIBMLP_SRCS:.cpp=.o) $(LIBREAD_SRCS:.cpp=.o) $(TEST_SRC:.cpp=.o)

TARGET = irm_mlp_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

libmlp/%.o: libmlp/%.cpp $(LIBMLP_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

libread/%.o: libread/%.cpp $(LIBREAD_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

irm/irm_test/%.o: irm/irm_test/%.cpp $(LIBMLP_HDRS) $(LIBREAD_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f libmlp/*.o libread/*.o irm/irm_test/*.o $(TARGET)