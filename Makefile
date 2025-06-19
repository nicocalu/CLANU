CXX = g++
CXXFLAGS = -Ilibmlp -Ilibread -Wall -Wextra -Ofast -std=c++20

# Source and header files
LIBMLP_SRCS := $(wildcard libmlp/*.cpp)
LIBMLP_HDRS := $(wildcard libmlp/*.h)
LIBREAD_SRCS := $(wildcard libread/*.cpp)
LIBREAD_HDRS := $(wildcard libread/*.h)

TEST_SRC := irm/irm_test/irm_mlp_test.cpp
SGD_SRC  := irm/irm_train_sgd/irm_mlp_train_sgd.cpp
ADAM_SRC := irm/irm_train_adam/irm_mlp_train_adam.cpp

OBJS_TEST := $(LIBMLP_SRCS:.cpp=.o) $(LIBREAD_SRCS:.cpp=.o) $(TEST_SRC:.cpp=.o)
OBJS_SGD  := $(LIBMLP_SRCS:.cpp=.o) $(LIBREAD_SRCS:.cpp=.o) $(SGD_SRC:.cpp=.o)
OBJS_ADAM := $(LIBMLP_SRCS:.cpp=.o) $(LIBREAD_SRCS:.cpp=.o) $(ADAM_SRC:.cpp=.o)

TARGET_TEST = irm_mlp_test
TARGET_SGD = irm_mlp_sgd
TARGET_ADAM = irm_mlp_adam

all: $(TARGET_TEST) $(TARGET_SGD) $(TARGET_ADAM)

$(TARGET_TEST): $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET_SGD): $(OBJS_SGD)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET_ADAM): $(OBJS_ADAM)
	$(CXX) $(CXXFLAGS) -o $@ $^

libmlp/%.o: libmlp/%.cpp $(LIBMLP_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

libread/%.o: libread/%.cpp $(LIBREAD_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

irm/irm_test/%.o: irm/irm_test/%.cpp $(LIBMLP_HDRS) $(LIBREAD_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f libmlp/*.o libread/*.o irm/irm_test/*.o irm/irm_train_sgd/*.o $(TARGET_TEST) $(TARGET_SGD) $(TARGET_ADAM)