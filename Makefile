CC = gcc
FLAGS = -g

SOURCE = bptree.c main.c
main: 
	${CC} ${SOURCE} ${FLAGS}

clean:
	rm -f a.out
