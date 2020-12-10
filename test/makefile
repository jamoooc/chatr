# makefile

# define paths
PATHU = unity/
PATHS = src/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

TARGET = test

COMPILE = gcc -c
LINK = gcc
CFLAGS = -I $(PATHS)
LIB = -lncurses
# TEST = ../unity/unity.c

# tmp set filepaths
_DEP = main.h utils.h message.h client.h connect.h pfds.h ui.h
DEP = $(patsubst %,$(PATHS)%,$(_DEP))

_OBJ = main.o utils.o message.o client.o connect.o pfds.o ui.c ../unity/unity.c
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




# test : # headers etc here when done properly
# 	gcc ../src/main.h test-something.c ../unity/unity.c -o test

../src/main.h ../src/utils.h ../src/message.h ../src/client.h ../src/connect.h ../src/pfds.h ../src/ui.h