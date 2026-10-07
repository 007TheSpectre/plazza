CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -Isrc
LDFLAGS  := -lpthread

NAME     := plazza

SRCS     := src/main.cpp \
            src/concurrency/Mutex.cpp \
            src/concurrency/CondVar.cpp \
            src/ipc/NamedPipe.cpp \
            src/pizza/Margarita.cpp \
            src/pizza/Regina.cpp \
            src/pizza/Americana.cpp \
            src/pizza/Fantasia.cpp \
            src/pizza/PizzaFactory.cpp \
            src/kitchen/Stock.cpp \
            src/kitchen/ThreadPool.cpp \
            src/kitchen/Kitchen.cpp \
            src/reception/Parser.cpp \
            src/reception/KitchenProxy.cpp \
            src/reception/Reception.cpp

OBJS     := $(SRCS:.cpp=.o)

BUILD_DIR   := tests/build
NAME_TESTS  := $(BUILD_DIR)/unit_tests

LIB_SRCS := $(filter-out src/main.cpp, $(SRCS))

TEST_SRCS := tests/test_parser.cpp \
             tests/test_pizza_factory.cpp \
             tests/test_pizza_types.cpp \
             tests/test_safe_queue.cpp \
             tests/test_thread_pool.cpp \
             tests/test_stock.cpp \
             tests/test_pizza_packet.cpp \
             tests/test_mutex_condvar.cpp \
             tests/test_named_pipe.cpp \
             tests/test_safe_queue_stress.cpp \
             tests/test_stock_concurrent.cpp \
             tests/test_thread_pool_stress.cpp \
             tests/test_parser_edge.cpp

LIB_TEST_OBJS  := $(addprefix $(BUILD_DIR)/, $(LIB_SRCS:.cpp=.o))
UNIT_TEST_OBJS := $(addprefix $(BUILD_DIR)/, $(TEST_SRCS:.cpp=.o))
ALL_TEST_OBJS  := $(LIB_TEST_OBJS) $(UNIT_TEST_OBJS)

GCOVR := gcovr --filter 'src/'

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests: $(NAME_TESTS)

$(NAME_TESTS): $(ALL_TEST_OBJS)
	$(CXX) $(CXXFLAGS) --coverage -o $@ $^ $(LDFLAGS) -lcriterion

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) --coverage -c $< -o $@

tests_run: $(NAME_TESTS)
	./$(NAME_TESTS)
	@printf '\n--- Line coverage ---\n'
	$(GCOVR) --txt-metric line
	@printf '\n--- Branch coverage ---\n'
	$(GCOVR) --txt-metric branch --print-summary

functional_tests: $(NAME)
	@bash tests/functional/run_tests.sh

docs:
	doxygen Doxyfile

clean:
	$(RM) $(OBJS)
	$(RM) -r $(BUILD_DIR)
	$(RM) -r coverage_html

fclean: clean
	$(RM) $(NAME)
	$(RM) tests_plazza
	$(RM) -r docs/doxygen

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re tests tests_run functional_tests docs
