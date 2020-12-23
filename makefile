# makefile
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=out
endif

# define paths
PATHU = unity/
PATHS = src/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

TARGET = chatr
TEST = build/tests.out
TEST_RUNNER = test/test-runner.c

CC = gcc
LINK = gcc
CFLAGS = -I $(PATHS)
LIBS = -lncurses

DEPENDS = $(patsubst %,$(PATHS)%,$(wildcard $(PATHS)*.h))
OBJECTS = $(patsubst $(PATHS)%.c,$(PATHO)%.o,$(wildcard $(PATHS)*.c))
TESTC = utils.c message.c client.c connect.c pfds.c ui.c # no main # for TESTS
TESTS = $(patsubst %,$(PATHS)%,$(TESTC)) # full path test source files

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR) $(PATHRF)

default: $(BUILD) $(TARGET) 

# link object files into executable
$(TARGET): $(OBJECTS)
	$(LINK) -o $@ $^ $(CFLAGS) $(LIBS)

# compile individual object files
$(PATHO)%.o:: $(PATHS)%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: test
.PHONY: clean

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`
RESULTS = $(PATHR)test-results.txt

test: $(BUILD_PATHS) clean tests.out
	@./$(TEST)

# @./$(TEST) > ./$(RESULTS)
# @echo "-----------------------\nIGNORES:\n-----------------------"
# @echo "$(IGNORE)"
# @echo "-----------------------\nFAILURES:\n-----------------------"
# @echo "$(FAIL)"
# @echo "-----------------------\nPASSED:\n-----------------------"
# @echo "$(PASSED)"
# @echo "\nDONE"

tests.out:
	@echo Compiling $@
	@$(CC) $(CFLAGS) $(TESTS) unity/unity.c $(TEST_RUNNER) -o $(PATHB)$@ $(LIBS)

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHD):
	$(MKDIR) $(PATHD)

$(PATHO):
	$(MKDIR) $(PATHO)

$(PATHR):
	$(MKDIR) $(PATHR)

clean:
	rm -f $(TARGET) $(PATHO)*.o $(TEST) $(RESULTS)
