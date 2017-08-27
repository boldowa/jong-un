# Enable gcov
CPPUTEST_USE_GCOV = Y

# Execute flags
CPPUTEST_EXE_FLAGS = -c

# Set asar Flags
CPPUTEST_LDFLAGS = -L. -Lbolib/build -lbolib -lasar -ldl

#---- Outputs ----#
COMPONENT_NAME = unko
#Set this to @ to keep the makefile quiet
SILENCE = @

#--- Inputs ----#
PROJECT_HOME_DIR = .
ifeq "$(CPPUTEST_HOME)" ""
    CPPUTEST_HOME = ../CppUTest
endif
CPP_PLATFORM = Gcc

SRC_DIRS = \
    src \
    src/*

# to pick specific files (rather than directories) use this:    
SRC_FILES = 

TEST_SRC_DIRS = \
    tests \
    tests/*

MOCKS_SRC_DIRS = \
    mocks \

INCLUDE_DIRS =\
  .\
  include \
  include/* \
  bolib/include \
  bolib/include/* \
  $(CPPUTEST_HOME)/include/ \
  $(CPPUTEST_HOME)/include/Platforms/Gcc\
  mocks

CPPUTEST_CFLAGS = -DUNIX -ansi
CPPUTEST_WARNINGFLAGS = -Wall -Werror -Wswitch-default 
CPPUTEST_WARNINGFLAGS += -Wconversion -Wswitch-enum 

include $(CPPUTEST_HOME)/build/MakefileWorker.mk

