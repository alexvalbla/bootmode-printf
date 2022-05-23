CC=gcc
CFLAGS=-Wall

SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR=bin
LIBDIR=lib
LIBNAME=libbmprint

BIN=$(BINDIR)/main
LIB=$(LIBDIR)/$(LIBNAME).a
HEADER=$(LIBDIR)/$(LIBNAME).h

all: $(LIB)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)
	cp $(SRC)/print.h $(HEADER)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -rf $(OBJ)/*
	rm -rf $(BINDIR)/*

clobber: clean
	rm -rf $(LIBDIR)/*
