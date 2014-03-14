all:
	gcc TCS.c -o TCS -lcurses
run:all
	./TCS -f program.bin
