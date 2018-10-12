echo "---------------- mmap compilation ---------------"
gcc mmap.c mylib.c -o mmap.exe -O3 -lpthread
echo "---------------- mmap compiled ------------------"
echo "-------------------------------------------------"
echo "---------------- msg compilation ---------------"
./recompile_msg.sh $1
echo "---------------- msg compiled ------------------"
echo "-------------------------------------------------"
echo "---------------- mmap compilation ---------------"
gcc shm.c mylib.c -o shm.exe -O3 -lpthread
echo "---------------- mmap compiled ------------------"
