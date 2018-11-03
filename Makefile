#Main Makefile
CC := gcc
CFLAGS := -Wall -MMD

#Directories
IDIR := ./include
SDIR := ./src
ODIR := ./obj/release

#Paths
INCLUDE_PATHS := -I$(IDIR)

#Libraries
LOADLIBES := -pthread

#Compilation line
COMPILE = $(CC) $(CFLAGS) $(INCLUDE_PATHS)

#FILEs
#---------------Source----------------#
SRCS := $(wildcard $(SDIR)/*.c)

#---------------Object----------------#
OBJS = $(SRCS:$(SDIR)/%.c=$(ODIR)/%.o)
#-------------Dependency--------------#
DEPS = $(SRCS:$(SDIR)/%.c=$(ODIR)/%.d)

# Build main application
all: $(OBJS)
	$(COMPILE) $(OBJS) main.c -o servidor $(LOADLIBES)

# Build main application for debug
debug: CFLAGS += -g -DDEBUG
debug: all


# Include all .d files
-include $(DEPS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(COMPILE) -c $< -o $@ $(LOADLIBES)

.PHONY : clean
clean :
	-rm -f obj/debug/* obj/release/* servidor cliente *.d

init:
	mkdir -p include
	mkdir -p src
	mkdir -p obj
	mkdir -p obj/windows
	mkdir -p obj/linux

run:
	./$(BIN)
