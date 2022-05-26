CC=gcc
# CFLAGS=-Wall

SRC=src
TEST=tests
OBJ=obj
BINDIR=bin

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
TESTS=$(wildcard $(TEST)/*.c)
BINS=$(patsubst $(TEST)/%.c, $(BINDIR)/%, $(TESTS))

LIBDIR=lib
LIB_BASE_NAME=bmprint
LIB=$(LIBDIR)/lib$(LIB_BASE_NAME).a
HEADER=$(LIBDIR)/$(LIB_BASE_NAME).h

all: $(LIB)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: all

$(LIB): $(OBJS) $(LIBDIR)
	ar rcs $@ $(OBJS)
	cp $(SRC)/print.h $(HEADER)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h $(OBJ)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/%: $(TEST)/%.c $(LIB) $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

$(OBJ):
	mkdir -p $@

$(LIBDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

test: $(BINS)
	for b in ${BINS} ; do ./$${b} ; done

clean:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)

clobber: clean
	rm -rf $(LIBDIR)
