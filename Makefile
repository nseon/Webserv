NAME		:=	webserv
CLIENT		:=	client

# ---------------FILES--------------- #

MAKE_DIR	:=	.make/
BUILD_DIR	:=	$(MAKE_DIR)build_$(shell git branch --show-current)/

SRC_DIR		:=	src/

OBJ			=	$(patsubst %.cpp, $(BUILD_DIR)%.o, $(SRC))
OBJ_CLIENT	=	$(patsubst %.cpp, $(BUILD_DIR)%.o, $(SRC_CLIENT))

DEP			=	$(patsubst %.cpp, $(BUILD_DIR)%.d, $(SRC))

# ---------------MAIN---------------- #

SRC			:=	server.cpp
SRC_CLIENT	:=	client.cpp

# --------------INCLUDES------------- #

INCS_DIR	:=	includes/
INCLUDES	:=	$(INCS_DIR)

# --------------CONFIGS-------------- #

CC			:=	c++
CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -Weverything -Wno-suggest-override -Wno-suggest-destructor-override -Wno-padded -Wno-address-of-temporary
CPPFLAGS	:=	-MMD -MP $(addprefix -I, $(INCLUDES))

MAKEFLAGS	+=	--no-print-directory --jobs

# ---------------MODES--------------- #

MODES := debug

MODE_TRACE	:= $(BUILD_DIR).mode_trace
LAST_MODE	:= $(shell cat $(MODE_TRACE) 2>/dev/null)

MODE ?= basic

BUILD_DIR := $(BUILD_DIR)$(MODE)/

ifeq ($(MODE), debug)
	CFLAGS = -g3 -std=c++98
endif

ifneq ($(LAST_MODE), $(MODE))
$(NAME): force
endif

# --------------TARGETS-------------- #

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	@echo $(MODE) > $(MODE_TRACE)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(CLIENT): $(OBJ_CLIENT)
	@echo $(MODE) > $(MODE_TRACE)
	$(CC) $(CFLAGS) $(OBJ_CLIENT) -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: $(MODES)
$(MODES):
	@$(MAKE) MODE=$@

.PHONY: clean
clean:
	rm -rf $(MAKE_DIR)

.PHONY: fclean
fclean:
	rm -rf $(NAME) $(MAKE_DIR)

.PHONY: re
re: fclean
	$(MAKE)
	
.PHONY: force
force:

-include $(DEP)

.DEFAULT_GOAL := all
