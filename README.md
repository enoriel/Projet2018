![Alt text](aero.png "Ynov Estei")
# Projet2018
RobotCup Soccer
Membre du groupe:
- CASTETS MATTHIEU
- EL MERNISSI YASSINE
- OFFOUGA JORIS
- REBILLON ERIC

# Contenu du projet:
- HCS08 -> Code pour microcontrôleur MC9S08QE128CLK
- Linux -> Gestion du MCP2210 Microchip & manette de Playstation 3

# Installation:
Prérequis:
```
sudo apt install joystick make build-essential
```

Pour compiler le programme: 
```
$cd src
$make
$sudo cp 99-mcp2210-spi2usb.rules /lib/udev/rules.d/99-mcp2210-spi2usb.rules
$sudo reboot
```
Pensez à brancher les appareils avant de lancer le programme (à la racine):
```
#$./controller /dev/input/js0 /dev/mcp2210
```
