#/bin/sh

make clean

make build
read -p "Compilazione eseguita. Premi invio per eseguire..."

gnome-terminal --geometry=100x30 -x sh -c "setterm -term linux -back blue -fore white; ./build/server; exec bash"  
gnome-terminal -x sh -c "setterm -term linux -back red -fore white; ./build/client; exec bash" 
gnome-terminal -x sh -c "setterm -term linux -back green -fore white; ./build/client; exec bash" 


alacritty -x sh -c "./build/server; exec bash"
alacritty -x sh -c "./build/client; exec bash"
alacritty -x sh -c "./build/client; exec bash"
