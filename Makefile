# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nseon <nseon@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/13 10:54:16 by nseon             #+#    #+#              #
#    Updated: 2026/04/03 16:06:51 by nseon            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:=	webserv
CLIENT		:=	client

# ---------------FILES--------------- #

MAKE_DIR	:=	.make/
BUILD_DIR	:=	$(MAKE_DIR)build_$(shell git branch --show-current)/

SRC_DIR		:=	src/

OBJ			=	$(patsubst %.cpp, $(BUILD_DIR)%.o, $(SRC))
CL_OBJ		=	$(patsubst %.cpp, $(BUILD_DIR)%.o, $(CL_SRC))

DEP			=	$(patsubst %.cpp, $(BUILD_DIR)%.d, $(SRC))

# ---------------MAIN---------------- #

SRC			:= server.cpp

CL_SRC		:= client.cpp

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
all: $(NAME) $(CLIENT)

$(NAME): $(OBJ)
	@echo $(MODE) > $(MODE_TRACE)
	$(CC) $(CFLAGS) $(OBJ) -o $@
	
$(CLIENT): $(CL_OBJ)
	@echo $(MODE) > $(MODE_TRACE)
	$(CC) $(CFLAGS) $(CL_OBJ) -o $@

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
