# Name of the library in all lowercase letters
PROG = hashmap

# Directories
SRC = src
HED = include
BIN = bin
VPATH := $(SRC):$(HED):$(BIN)

# Files
LIB = lib$(PROG).so
SRCS := $(wildcard $(SRC)/*.c)
HEDS := $(wildcard $(HED)/*.h)
OBJS := $(addprefix $(BIN)/,$(notdir $(SRCS:%.c=%.o)))

# Compilation options
CFLAGS := -std=c99 -Wall -Wpedantic -I$(SRC) -I$(HED) -I$(BIN) -O2


##############
# Make Rules #
##############
.PHONY: all $(PROG) clean move $(BIN)

all: $(PROG) move

$(PROG): $(LIB)

$(LIB): $(OBJS) | $(BIN)
	gcc -g -shared $(OBJS) -o $(BIN)/$(LIB)

$(BIN)/%.o: $(SRC)/%.c $(HED)/%.h | $(BIN)
	gcc -g $(CFLAGS) -c -fpic $< -o $@


#############
# Utilities #
#############

clean:
	rm -f ../$(LIB) $(BIN)/*.o $(BIN)/$(LIB)

move:
	mv $(BIN)/$(LIB) ../

$(BIN):
	mkdir -p $@

