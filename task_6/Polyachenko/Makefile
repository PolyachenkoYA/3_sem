all: compile run
compile: main.c Makefile
	gcc main.c -o backup.exe
run:
	@echo --------------------------------
	./backup.exe
	@echo --------------------------------    
clean:
	rm backup.exe || true
