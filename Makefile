# Name of the library in all lowercase letters
PROG = hashmap

# Directories
SRC = src
HED = include
BIN = bin
TEST_DIR = tests
VPATH := $(SRC):$(HED):$(BIN)

# Files
LIB = lib$(PROG).so
SRCS := $(wildcard $(SRC)/*.c)
HEDS := $(wildcard $(HED)/*.h)
OBJS := $(addprefix $(BIN)/,$(notdir $(SRCS:%.c=%.o)))
TESTS := $(patsubst %.c,%,$(wildcard $(TEST_DIR)/*.c))

# Compilation options
CFLAGS := -std=c99 -Wall -Wpedantic -I$(SRC) -I$(HED) -I$(BIN) -O2


##############
# Make Rules #
##############
.PHONY: all $(PROG) clean move tests

all: $(PROG) tests move

$(PROG): $(LIB)

$(LIB): $(OBJS) | $(BIN)
	gcc -g -shared $(OBJS) -o $(BIN)/$(LIB)

$(BIN)/%.o: $(SRC)/%.c $(HED)/%.h | $(BIN)
	gcc -g $(CFLAGS) -c -fpic $< -o $@

tests: $(TESTS)

$(TESTS): %: %.c | $(LIB)
	gcc -g $(CFLAGS) -L$(BIN) -l$(PROG) $< -o $@


#############
# Utilities #
#############

clean:
	rm -f ../$(LIB) $(BIN)/*.o $(BIN)/$(LIB) $(TESTS)

move:
	mv $(BIN)/$(LIB) ../

$(BIN):
	mkdir -p $@

echo:
	@echo "Test Dir = $(TEST_DIR)"
	@echo "Tests    = $(TESTS)"

