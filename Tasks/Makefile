CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lssl -lcrypto

all: task1 task2

task1: task1.c
	$(CC) $(CFLAGS) -o task1 task1.c $(LIBS)

task2: task2.c
	$(CC) $(CFLAGS) -o task2 task2.c $(LIBS)

run1: task1
	./task1

run2: task2
	./task2

task5: task5.c
	$(CC) $(CFLAGS) -o task5 task5.c $(LIBS)

run5: task5
	./task5

run7:
	python3 task7.py

clean:
	rm -f task1 task2