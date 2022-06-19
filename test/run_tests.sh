#! /bin/bash

cd ../receiver/
make clean
make
gnome-terminal -e "./receiver.out localhost 8004 8003"

cd ../router/
make clean
make
gnome-terminal -e "./router.out localhost 8002 8001 8003 8004 "

cd ../sender/
make clean
make
for i in {1..20}
do
    gnome-terminal -e "./sender.out $i localhost 8001 8002"   
done

