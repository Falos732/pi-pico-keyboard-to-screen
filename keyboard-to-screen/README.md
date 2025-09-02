https://github.com/byteit101/pio-debugger-emulator/
java -jar ./pio-debugger-emulator-main/app/build/libs/app.jar server
java -jar ./pio-debugger-emulator-main/app/build/libs/app.jar gui

Main = SingleApp.java


server	EmulationServer	Lance un serveur d’émulation, probablement pour interagir avec des clients externes ou une interface distante.  
monitor	Monitor	Interface en ligne de commande pour charger, modifier, exécuter et inspecter des programmes PIO. Supporte l’historique et l’autocomplétion.  
fifoobserver	FifoObserver	Observe les registres FIFO (TX/RX) pour détecter les débordements, sous-débordements ou comportements inattendus.  
codeobserver	CodeObserver	Permet de suivre l’exécution du code PIO instruction par instruction, utile pour le pas-à-pas et le traçage.  
gpioobserver	GPIOObserver	Visualise l’état des broches GPIO, y compris les changements de direction ou de niveau logique.  
diagram	Diagram	Génère des diagrammes temporels à partir de l’émulation, très utile pour documenter ou comprendre le timing.  
doctool	RegistersDocsBuilder	Génère de la documentation sur les registres internes du PIO (X, Y, ISR, OSR, etc.).  
observer	Observer	Outil générique d’observation, peut-être une base commune pour les autres observateurs.  
gui	MultiGuiObserver	Interface graphique multi-vue pour visualiser l’état interne du PIO, les GPIO, les FIFO, etc. Permet aussi de recharger des scripts dynamiquement.