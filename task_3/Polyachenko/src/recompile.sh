echo "----------------- cmp compilation ---------------"
gcc cmp.c mylib.c -o cmp.exe -O3
echo "----------------- cmp compiled ------------------"
echo "-------------------------------------------------"
echo "---------------- gen compilation ----------------"
gcc gen.c mylib.c -o gen.exe -O3
echo "---------------- gen compiled -------------------"
echo "-------------------------------------------------"
echo "---------------- mmap compilation ---------------"
gcc mmap.c mylib.c -o mmap.exe -O3
echo "---------------- mmap compiled ------------------"
echo "-------------------------------------------------"
echo "---------------- msg compilation ---------------"
./recompile_msg.sh $1
echo "---------------- msg compiled ------------------"