echo "#define BUF_SIZ "$1 > BUF_SIZ.h
gcc msg.c mylib.c -o msg.exe -O3
