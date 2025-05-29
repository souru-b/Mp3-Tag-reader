OBJ=$(patsubst %.c,%.o,$(wildcard *.c))
mp3.exe : $(OBJ)
	gcc -o $@ $^
clean :
	rm *.o *.exe

