CPP         =   c++
FLAGS       =   -Wall -Wextra -Werror -std=c++98 -I../inc -Iinc
SRC_DIR     =   ../src
TEST_DIR    =   src
OBJ_DIR     =   build

# |--- 0. DEPENDENCIES --------------------------------------------------------|

DEP_SRCS		=	checkExtension.cpp \
					parseConfig.cpp \
					parseConfigUtils.cpp \
					readFile.cpp \
					AutoIndex.cpp \
					Block.cpp \
					Cgi.cpp \
					Config.cpp \
					Server.cpp \
					Client.cpp \
					ClientStateHandling.cpp \
					ClientDelete.cpp \
					ClientError.cpp \
					ClientGet.cpp \
					ClientPost.cpp \
					Location.cpp \
					Log.cpp \
					signal.cpp \
					readRequest.cpp \
					HttpRequest.cpp \
					HttpResponse.cpp

DEP_OBJS		=	$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(DEP_SRCS))

# |--- 1. UNIT TESTS ----------------------------------------------------------|

UNIT_TEST_FILES	=	unit_test.cpp \
					testCheckExtension.cpp \
                	testParseDirectives.cpp \
					testReadFile.cpp \
					testReadRequest.cpp \
					testHttp.cpp

UNIT_TEST_DIR	=	src/unit_test/
UNIT_TEST_SRCS	=	$(addprefix $(UNIT_TEST_DIR), $(UNIT_TEST_FILES))
UNIT_OBJS		=	$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(UNIT_TEST_FILES))

# |--- 2. INTEGRATION TESTS ---------------------------------------------------|

INTEG_TEST_FILES=	integ_test.cpp \
					testCgiFileErrors.cpp \
					testCgiGet.cpp \
					testCgiPost.cpp \
					createTestScript.cpp

INTEG_TEST_DIR	=	src/integ_test/
INTEG_TEST_SRCS	=	$(addprefix $(INTEG_TEST_DIR), $(INTEG_TEST_FILES))
INTEG_OBJS		=	$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(INTEG_TEST_FILES))

# |--- 3. RULES ---------------------------------------------------------------|

all: unit integration

unit: $(OBJ_DIR) $(UNIT_OBJS) $(DEP_OBJS)
		@$(CPP) $(FLAGS) $(UNIT_OBJS) $(DEP_OBJS) -o unit_test
		@echo "✅ Built unit_test"

integration: $(OBJ_DIR) $(INTEG_OBJS) $(DEP_OBJS)
		@$(CPP) $(FLAGS) $(INTEG_OBJS) $(DEP_OBJS) -o integ_test
		@echo "✅ Built integ_test"

$(OBJ_DIR)/%.o: $(UNIT_TEST_DIR)/%.cpp
		@mkdir -p $(dir $@)
		@$(CPP) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(INTEG_TEST_DIR)/%.cpp
		@mkdir -p $(dir $@)
		@$(CPP) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
		@mkdir -p $(dir $@)
		@$(CPP) $(FLAGS) -c $< -o $@

$(OBJ_DIR):
		@mkdir -p $(OBJ_DIR)

clean:
		@rm -rf $(OBJ_DIR)

fclean: clean
		@rm -f unit_test integ_test

re: fclean all

.PHONY: all unit integration clean fclean re
