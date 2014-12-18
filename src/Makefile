test:
		gcc -c master.c -o master.o
		gcc -c monitor.c -o monitor.o
		gcc -c main.c -o main.o
		gcc -o main master.o monitor.o main.o
clean:
		rm -rf *.o main
all:clean test

