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
LIBS = -lncurses

# tmp set filepaths
_DEP = main.h utils.h message.h client.h connect.h pfds.h ui.h
DEP = $(patsubst %,$(PATHS)%,$(_DEP))

_OBJ = main.o utils.o message.o client.o connect.o pfds.o ui.c
OBJ = $(patsubst %,$(PATHS)%,$(_OBJ))

$(PATHS)%.o: %.c $(DEP)
		$(COMPILE) -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
		$(LINK) -o $@ $^ $(CFLAGS) $(LIBS)

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

.PHONY: test
test:
	gcc -o test/test-client ./test/test-client.c ./src/utils.c ./src/client.c ./src/message.c ./src/pfds.c ./src/connect.c ./src/ui.c ./unity/unity.c -lncurses
	gcc -o test/test-message ./test/test-message.c ./src/utils.c ./src/client.c ./src/message.c ./src/pfds.c ./src/connect.c ./src/ui.c ./unity/unity.c -lncurses
	gcc -o test/test-utils ./test/test-utils.c ./src/utils.c ./src/client.c ./src/message.c ./src/pfds.c ./src/connect.c ./src/ui.c ./unity/unity.c -lncurses
	gcc -o test/test-pfds ./test/test-pfds.c ./src/utils.c ./src/client.c ./src/message.c ./src/pfds.c ./src/connect.c ./src/ui.c ./unity/unity.c -lncurses
	
	-./test/test-client > build/results/test-client.txt
	-./test/test-message > build/results/test-message.txt
	-./test/test-utils > build/results/test-utils.txt
	-./test/test-pfds > build/results/test-pfds.txt

	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"

.PHONY: clean
clean:
		rm -f $(TARGET) $(PATHS)*.o
