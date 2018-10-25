all: compile run
compile: main.c mylib.c mylib.h Makefile
	gcc main.c mylib.c -o 1.exe -pthread
run:
	./1.exe 100 tst
clean:
	rm 1.exe || true
