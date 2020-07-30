# all, clean, move, and echo are not filenames
.PHONY: all clean move echo

# Directories
SRC = src
HED = include
BIN = bin
VPATH := $(SRC):$(HED):$(BIN)

# Files
LIB = libhashmap.so
SRCS := $(wildcard $(SRC)/*.c)
HEDS := $(wildcard $(HED)/*.h)
OBJS := $(addprefix $(BIN)/,$(notdir $(SRCS:%.c=%.o)))

# Compilation options
CFLAGS := -std=c99 -Wall -Wpedantic -I$(SRC) -I$(HED) -I$(BIN)


##############
# Make Rules #
##############

all: $(LIB) move

$(LIB): $(OBJS)
	gcc -g -shared $(OBJS) -o $(BIN)/$(LIB)

$(BIN)/%.o: $(SRC)/%.c $(HED)/%.h
	gcc -g $(CFLAGS) -c -fpic $< -o $@


#############
# Utilities #
#############

clean:
	rm -f ../$(LIB) $(BIN)/*.o $(BIN)/$(LIB)

move:
	mv $(BIN)/$(LIB) ../

echo:
	@echo SRC = $(SRC)
	@echo HED = $(HED)
	@echo BIN = $(BIN)
	@echo
	@echo SRCS = $(SRCS)
	@echo HEDS = $(HEDS)
	@echo OBJS = $(OBJS)
	@echo
	@echo LIB = $(LIB)

