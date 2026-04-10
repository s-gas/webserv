# |--- Variables --------------------------------------------------------------|

NAME		= webserv

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -I$(INC_DIR) -MMD -MP

# Colors
GREEN		= \033[0;32m
BLUE		= \033[0;36m
RESET		= \033[0m

# Directories
SRC_DIR		= src
OBJ_DIR		= obj
INC_DIR		= inc

# Source files
SRC_FILES	= $(addsuffix .cpp, main \
								Block \
								Config \
								Server \
								Location \
								Log \
								checkExtension \
								parseDirectives \
								parseUtils \
								readFile \
				)
SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object and Dependency files
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS		= $(OBJS:.o=.d)

# |--- Rules ------------------------------------------------------------------|

all: $(NAME)

# Main Target
$(NAME): $(OBJS)
	@echo "$(BLUE)|--Linking $(NAME)----------------|$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)|--$(NAME) created successfully!--|$(RESET)"

# Compiling .cpp to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# |--- CleanUp ----------------------------------------------------------------|

# Remove all Object-Files
clean:
	@echo "$(BLUE)|--Cleaning Objects---------------|$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)|--Cleaned successfully!----------|$(RESET)"

# Remove all Object- and Archive-Files
fclean: clean
	@echo "$(BLUE)|--Cleaning Executables-----------|$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)|--Cleaned successfully!----------|$(RESET)"

# Rebuild
re: fclean all

# |--- CleanUp ----------------------------------------------------------------|

# Include .d files
-include $(DEPS)

.PHONY: clean all re fclean
