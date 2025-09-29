https://github.com/byteit101/pio-debugger-emulator/    
cd pio-debugger-emulator-main
./gradlew jar  
java -jar ./pio-debugger-emulator-main/app/build/libs/app.jar server  
java -jar ./pio-debugger-emulator-main/app/build/libs/app.jar gui  
java -jar ./pio-debugger-emulator-main/app/build/libs/app.jar monitor

assemble -i C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/keyboard-to-screen/src/main.pio -o C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/keyboard-to-screen/src/main.hex -t C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/Pico-sdk/pico-sdk-tools/pioasm.exe

sinon : C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/Pico-sdk/pico-sdk-tools/pioasm.exe -o hex C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/keyboard-to-screen/src/main.pio C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/keyboard-to-screen/src/main.hex

si je veut directement changer ou aller chercher pioasm je peut modifier la ligne 122 du fichier [Assemble](..\pio-debugger-emulator-main\app\src\main\java\org\soundpaint\rp2040pio\monitor\commands\Assemble.java)  


load le programme dans la memoire (de base id 0, peut mettre 1)
load -f C:/Users/Utilisateur/Desktop/pi-pico-keyboard-to-screen/keyboard-to-screen/src/main.hex -p 0 -a 0x00

enable un machine a état de la memoire
sm -p 0 -s 0 +e

reset

pas necessaire
pinctrl -p 0 -s 0 --set-count=1 --set-base=0

initialize le gpio 0 pour le PIO 0
gpio -p 0 -g 0 -i
set le gpio 0 du pio 0 en sortie
gpio -p 0 -g 0 -e


simule 100 cycle de la memoire et la machine  
trace -p 0 -s 0 -c 100  
autre facons : 
trace -p 0 -s 0 -c 50 -i (montre les pointeurs)
trace -p 0 -c 50 -l (montre les GPIO local)
trace -p 0 -c 50 -g (montre les GPIO global)
trace -p 0 -s 0 -c 20 -w 10 (delai entre les cycle)

breakpoints -p 0 -s 0 -a 0x00


Main = SingleApp.java

- server	EmulationServer	Lance un serveur d’émulation, probablement pour interagir avec des clients externes ou une interface distante.  
- monitor	Monitor	Interface en ligne de commande pour charger, modifier, exécuter et inspecter des programmes PIO. Supporte l’historique et l’autocomplétion.  
- fifoobserver	FifoObserver	Observe les registres FIFO (TX/RX) pour détecter les débordements, sous-débordements ou comportements inattendus.  
- codeobserver	CodeObserver	Permet de suivre l’exécution du code PIO instruction par instruction, utile pour le pas-à-pas et le traçage.  
- gpioobserver	GPIOObserver	Visualise l’état des broches GPIO, y compris les changements de direction ou de niveau logique.  
diagram	Diagram	Génère des diagrammes temporels à partir de l’émulation, très utile pour documenter ou comprendre le timing.  
- doctool	RegistersDocsBuilder	Génère de la documentation sur les registres internes du PIO (X, Y, ISR, OSR, etc.).  
observer	Observer	Outil générique d’observation, peut-être une base commune pour les autres observateurs.  
- gui	MultiGuiObserver	Interface graphique multi-vue pour visualiser l’état interne du PIO, les GPIO, les FIFO, etc. Permet aussi de recharger des scripts dynamiquement.


pour compiler du code PIO : https://github.com/raspberrypi/pico-sdk  
https://github.com/raspberrypi/pico-setup-windows/releases/latest/download/pico-setup-windows-x64-standalone.exe  

extension necessaire : Java, plateformIO

cmake .. -G Ninja -DPICO_BOARD=pico -DPICO_SDK_FETCH_FROM_GIT=on

git clone https://github.com/raspberrypi/pico-sdk.git
git submodule update --init

git clone https://github.com/sekigon-gonnoc/Pico-PIO-USB.git

https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-mingw-w64-x86_64-arm-none-eabi.exe