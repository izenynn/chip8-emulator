# **************************************************************************** #
#                                     VARS                                     #
# **************************************************************************** #

# COLORS
NOCOL	= \033[0m
RED		= \033[31m
GRN		= \033[32m
YEL		= \033[33m
BLU		= \033[34m
MAG		= \033[35m
CYN		= \033[36m
LBLU	= \033[36m
LGRN	= \033[0;90m

# OS
UNAME_S := $(shell uname -s)

# **************************************************************************** #
#                                   PROGRAM                                    #
# **************************************************************************** #

BIN_NAME = emulator

INSTALL_PATH ?= /usr/local/bin

# **************************************************************************** #
#                                   COMPILER                                   #
# **************************************************************************** #

MAKE = make

CXX = g++
#CXX = clang++

CXXFLAGS += -std=c++17 -Wall -Wextra -Werror -MD
CXXFLAGS += -m64

# **************************************************************************** #
#                                    PATHS                                     #
# **************************************************************************** #

SRC_PATH = src
INC_PATH = inc
OBJ_PATH = obj
LIB_PATH = lib
BIN_PATH = bin

# **************************************************************************** #
#                                    FLAGS                                     #
# **************************************************************************** #

CXXFLAGS += -I ./$(INC_PATH)
CXXFLAGS += -I ./$(SRC_PATH)

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRC_FILES =	main.cc					\
			chip8.cc				\
			engine.cc				\
			window.cc				\

OBJ_FILES = $(SRC_FILES:%.cc=%.o)
DEP_FILES = $(SRC_FILES:%.cc=%.d)

SRC = $(addprefix $(SRC_PATH)/, $(SRC_FILES))
OBJ = $(addprefix $(OBJ_PATH)/, $(OBJ_FILES))
DEP = $(addprefix $(OBJ_PATH)/, $(DEP_FILES))

# **************************************************************************** #
#                                     LIBS                                     #
# **************************************************************************** #

# SDL2
SDL2_NAME = libSDL2.a
SDL2MAIN_NAME = libSDL2main.a
SDL2_PATH = $(LIB_PATH)/SDL2

CXXFLAGS += -I ./$(SDL2_PATH)/include
LDFLAGS += -L ./$(SDL2_PATH)

LDLIBS += -lSDL2 -lSDL2main

# SYSTEM LIBS
LDLIBS += -ldl -lpthread

# **************************************************************************** #
#                                      OS                                      #
# **************************************************************************** #

CXXFLAGS += -I $(LIB_PATH)/include/

RM = rm -rf
CP = cp
PRINTF = printf
UNAME_S := $(shell uname -s)
MKDIR = mkdir -p
SLASH = /
ifeq ($(UNAME_S),Linux)
	CXXFLAGS += -D LINUX
endif

NAME := $(BIN_PATH)/$(BIN_NAME)

# **************************************************************************** #
#                                    RULES                                     #
# **************************************************************************** #

# ALL
PHONY := all
all: $(NAME)

# NAME
$(NAME): $(OBJ) | $(BIN_PATH)
	@$(PRINTF) "\n${YEL}LINKING:${NOCOL}\n"
	@$(PRINTF) "${BLU}"
	$(CXX) $(CXXFLAGS) $(DEBUG) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)
	#$(CXX) $(DEBUG) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)
	@$(PRINTF) "${NOCOL}"
	@$(PRINTF) "\n${GRN}SUCCESS!${NOCOL}\n"
	@$(PRINTF) "${CYN}type \"./${NAME}\" or \"make run\" to start!${NOCOL}\n"

# RELEASE
PHONY += release
release: DEBUG := -O3 -D NDEBUG
release: $(NAME)

# SANITIZE ADDRESS
PHONY += sanitize
ifeq ($(UNAME_S),Linux)
sanitize: DEBUG := -pedantic -g3 -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=bounds -fsanitize=null
endif
ifeq ($(UNAME_S),Darwin)
sanitize: DEBUG := -pedantic -g3 -fsanitize=address
endif
sanitize: $(NAME)

# SANITIZE THREAD
PHONY += thread
thread: DEBUG := -g3 -fsanitize=thread
thread: $(NAME)

# VALGRIND
valgrind: DEBUG := -ggdb3
valgrind: $(NAME)

# OBJ
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cc | $(OBJ_PATH)
	@$(PRINTF) "${BLU}"
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@
	@$(PRINTF) "${NOCOL}"

# OBJ PATH
$(OBJ_PATH):
	@$(PRINTF) "${MAG}"
	$(MKDIR) $@
	@$(PRINTF) "${NOCOL}"

# BIN_PATH
$(BIN_PATH):
	@$(PRINTF) "${MAG}"
	$(MKDIR) $@
	@$(PRINTF) "${NOCOL}"

# CLEAN
PHONY += clean
clean:
	@$(PRINTF) "${RED}"
	$(RM) $(OBJ_PATH)
	@$(PRINTF) "${NOCOL}"

# FULL CLEAN
PHONY += fclean
fclean: clean
	@$(PRINTF) "${RED}"
	$(RM) $(firstword $(subst $(SLASH), , $(BIN_PATH)))
	@$(PRINTF) "${NOCOL}"

# RE
PHONY += re
re: fclean all

# RUM
PHONY += run
run: $(NAME)
	@$(PRINTF) "\n${YEL}RUNNING...${NOCOL}\n"
	$(NAME)
	@$(PRINTF) "\n${GRN}SUCCESS!${NOCOL}\n"

-include $(DEP)

.PHONY: $(PHONY)
