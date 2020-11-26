# makefile

# define paths
PATHU = unity/
PATHS = src/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

TARGET = chatr

COMPILE = gcc -c
LINK = gcc
CFLAGS = -I $(PATHS)
LIB = -lncurses

# tmp set filepaths
_DEP = main.h utils.h message.h client.h connect.h pfds.h
DEP = $(patsubst %,$(PATHS)%,$(_DEP))

_OBJ = main.o utils.o message.o client.o connect.o pfds.o
OBJ = $(patsubst %,$(PATHS)%,$(_OBJ))

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

$(PATHS)%.o: %.c $(DEP)
		$(COMPILE) -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
		$(LINK) -o $@ $^ $(CFLAGS) $(LIB) 

# print:
# 	@echo "this is a test"

.PHONY: clean
clean:
		rm -f $(TARGET) $(PATHS)*.o
