CXX = g++
CXXFLAGS = -std=c++14 -O3 -Wall -Wextra -Iinclude

SRCS = src/network.cpp src/inventory_policy.cpp src/discrete_event_sim.cpp src/spare_parts_optimizer.cpp src/warranty_model.cpp
MAIN = src/main.cpp
TEST_SRCS = tests/test_suite.cpp

TARGET = optichain_engine
TEST_TARGET = run_tests

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(SRCS) $(MAIN)
	$(CXX) $(CXXFLAGS) $(SRCS) $(MAIN) -o $(TARGET)

$(TEST_TARGET): $(SRCS) $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(TEST_SRCS) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe $(TEST_TARGET) $(TEST_TARGET).exe *.o

.PHONY: all test clean
