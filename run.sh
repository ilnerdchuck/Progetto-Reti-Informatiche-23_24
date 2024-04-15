#/bin/sh

make clean

make build
read -p "Compilazione eseguita. Premi invio per eseguire..."

gnome-terminal -x sh -c "./build/server; exec bash"
gnome-terminal -x sh -c "./build/client; exec bash"
gnome-terminal -x sh -c "./build/client; exec bash"


alacritty -x sh -c "./build/server; exec bash"
alacritty -x sh -c "./build/client; exec bash"
alacritty -x sh -c "./build/client; exec bash"
