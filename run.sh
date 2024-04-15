#/bin/sh

make clean

make build
read -p "Compilazione eseguita. Premi invio per eseguire..."

gnome-terminal -x sh -c "./build/server; exec bash" -t "SERVER" -geometry 100x30 
gnome-terminal -x sh -c "./build/client; exec bash" -t "CLIENT-1"
gnome-terminal -x sh -c "./build/client; exec bash" -t "CLIENT-2"


alacritty -x sh -c "./build/server; exec bash"
alacritty -x sh -c "./build/client; exec bash"
alacritty -x sh -c "./build/client; exec bash"
