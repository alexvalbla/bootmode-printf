NAME = libprintf.a
HEADER = libprintf.h
files = conversion.c formatting.c print.c
file_objects = $(files:.c=.o)
release_flags = -Wall -Wextra -Werror -O3

all: $(NAME)

$(NAME): $(file_objects)
	ar rcs $(NAME) $(file_objects)
	cat print.h > $(HEADER)

%.o: %.c %.h
	gcc $(release_flags) -c $<

clean:
	rm -f *.o

fclean:	clean
	rm -f $(NAME)
	rm -f $(HEADER)

re:
	make clean
	make all
