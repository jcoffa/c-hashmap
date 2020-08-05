.PHONY: all clean move bin hashmap

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
CFLAGS := -std=c99 -Wall -Wpedantic -I$(SRC) -I$(HED) -I$(BIN) -O2


##############
# Make Rules #
##############

all: hashmap move

hashmap: $(LIB)

$(LIB): $(OBJS) | bin
	gcc -g -shared $(OBJS) -o $(BIN)/$(LIB)

$(BIN)/%.o: $(SRC)/%.c $(HED)/%.h | bin
	gcc -g $(CFLAGS) -c -fpic $< -o $@



#############
# Utilities #
#############

clean:
	rm -f ../$(LIB) $(BIN)/*.o $(BIN)/$(LIB)

move:
	mv $(BIN)/$(LIB) ../

bin:
	mkdir -p $@

