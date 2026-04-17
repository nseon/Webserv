# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nseon <nseon@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/13 10:54:16 by nseon             #+#    #+#              #
#    Updated: 2026/03/30 14:48:25 by nseon            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:=	webserv

# ---------------FILES--------------- #

MAKE_DIR	:=	.make/
BUILD_DIR	:=	$(MAKE_DIR)build_$(shell git branch --show-current)/

SRC_DIR		:=	src/
TEST_DIR	:= tests/

OBJ			=	$(patsubst %.cpp, $(BUILD_DIR)%.o, $(SRC))

DEP			=	$(patsubst %.cpp, $(BUILD_DIR)%.d, $(SRC))

# ---------------MAIN---------------- #

BASE_SRC	:=	ASocket.cpp ListenerSocket.cpp ClientSocket.cpp PollingManager.cpp
MAIN		:=	main.cpp
SRC			:=	$(MAIN) $(BASE_SRC)

# ----------------TEST--------------- #

TEST_SRC	:=	main_test.cpp

# --------------INCLUDES------------- #

INCS_DIR	:=	includes/
INCLUDES	:=	$(INCS_DIR)

# --------------CONFIGS-------------- #

CXX			:=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
CPPFLAGS	:=	-MMD -MP $(addprefix -I, $(INCLUDES))

MAKEFLAGS	+=	--no-print-directory --jobs

# ---------------MODES--------------- #

MODES := debug test

MODE_TRACE	:= $(BUILD_DIR).mode_trace
LAST_MODE	:= $(shell cat $(MODE_TRACE) 2>/dev/null)

MODE ?= basic

BUILD_DIR := $(BUILD_DIR)$(MODE)/

ifeq ($(MODE), debug)
	CXXFLAGS = -g3 -std=c++98
endif

ifeq ($(MODE), test)
	INCLUDES += lib/
	SRC := $(TEST_SRC) $(BASE_SRC)
	CPPFLAGS	:=	-MMD -MP $(addprefix -I, $(INCLUDES))
	CXXFLAGS	= -Wall -Wextra -Werror
endif

ifneq ($(LAST_MODE), $(MODE))
$(NAME): force
endif

# --------------TARGETS-------------- #

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	@echo $(MODE) > $(MODE_TRACE)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)%.o: $(TEST_DIR)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

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
