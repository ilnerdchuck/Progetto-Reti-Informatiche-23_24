#/bin/sh

make clean

make build
read -p "Compilazione eseguita. Premi invio per eseguire..."

gnome-terminal --geometry=80x30 -x sh -c "setterm -term linux -back blue -fore white; ./build/server; exec bash" -t "SERVER" 
gnome-terminal -x sh -c "setterm -term linux -back red -fore white; ./build/client; exec bash" -t "CLIENT-1"
gnome-terminal -x sh -c "setterm -term linux -back brown -fore white; ./build/client; exec bash" -t "CLIENT-2"


alacritty -x sh -c "./build/server; exec bash"
alacritty -x sh -c "./build/client; exec bash"
alacritty -x sh -c "./build/client; exec bash"
