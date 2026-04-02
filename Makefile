# |--- Variables --------------------------------------------------------------|
NAME        = webserv
TEST_NAME   = tester
CXX         = c++
CXXFLAGS    = -Wall -Werror -Wextra -std=c++98 -I$(INC_DIR) -MMD -MP
# Colors
GREEN       = \033[0;32m
BLUE        = \033[0;36m
RESET       = \033[0m
# Directories
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = inc
TEST_DIR    = tests
# Source files
SRC_FILES   = $(addsuffix .cpp, main Server)
SRCS        = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
# Object and Dependency files
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS        = $(OBJS:.o=.d)
# Test files and objects
TEST_SRCS   = testCheckExtension.cpp
TEST_FILES  = $(addprefix $(TEST_DIR)/, $(TEST_SRCS))
TEST_OBJS   = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_FILES))
# Manual list of src files the tests actually need
TEST_DEPS   = $(OBJ_DIR)/checkExtension.o
# |--- Rules ------------------------------------------------------------------|
all: $(NAME)
# Main Target
$(NAME): $(OBJS)
	@echo "$(BLUE)|--Linking $(NAME)----------------|$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)|--$(NAME) created successfully!--|$(RESET)"
# Test Target
test: $(TEST_DEPS) $(TEST_OBJS)
	@echo "$(BLUE)|--Linking $(TEST_NAME)-----------|$(RESET)"
	@$(CXX) $(CXXFLAGS) $(TEST_OBJS) $(TEST_DEPS) -o $(TEST_NAME)
	@echo "$(GREEN)|--Running tests------------------|$(RESET)"
	@./$(TEST_NAME)
# Compiling .cpp to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
# Compiling test .cpp to .o
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
# |--- CleanUp ----------------------------------------------------------------|
clean:
	@echo "$(BLUE)|--Cleaning Objects---------------|$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)|--Cleaned successfully!----------|$(RESET)"
fclean: clean
	@echo "$(BLUE)|--Cleaning Executables-----------|$(RESET)"
	@rm -f $(NAME) $(TEST_NAME)
	@echo "$(GREEN)|--Cleaned successfully!----------|$(RESET)"
re: fclean all
# |--- Misc -------------------------------------------------------------------|
-include $(DEPS)
.PHONY: clean all re fclean test