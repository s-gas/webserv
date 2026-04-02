CPP         =   c++
FLAGS       =   -Wall -Wextra -Werror -std=c++98 -I../inc -Iinc
SRC_DIR     =   ../src
TEST_DIR    =   src
OBJ_DIR     =   build

TEST_SRCS   =   tests.cpp \
                testCheckExtension.cpp \
                testCheckDirectiveBlocks.cpp \
				testReadFile.cpp

DEP_SRCS    =   checkExtension.cpp \
                checkDirectiveBlocks.cpp \
				readFile.cpp

TEST_OBJS   =   $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))
DEP_OBJS    =   $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(DEP_SRCS))

NAME        =   tester

all: $(NAME)

$(NAME): $(TEST_OBJS) $(DEP_OBJS)
	@$(CPP) $(TEST_OBJS) $(DEP_OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CPP) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CPP) $(FLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re