#! /bin/bash

echo "Avviare il server *parallelo* in un altro terminale!"

sleep 3

for i in {0..100}
do
  ./gara_podistica_client -s 127.0.0.1 -i corridore_${i} &
done