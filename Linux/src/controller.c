/*
 * Controlle  de la manette
 * Enoriel <yassine.elmernssi@ynov.com>
 * Optimisé pour Linux Debian Stretch 9
 * Description:
 * Gestion de la manette de Playsation 3, du convertisseur USB vers SPI MCP2210-I/SO et du module radio nrf24l01+
 * Prérequis:
 * 			- SPIdev (voir documentaion: https://www.kernel.org/doc/Documentation/spi/spidev)
 * 			- Joystick (Pour debian: sudo apt install joystick)
 * 			- Driver MCP2210 (Projet de librairie C/C++: https://github.com/kerrydwong/MCP2210-Library)
 * 			- USB HID
 * 			- Datasheet du convertisseur (http://ww1.microchip.com/downloads/en/DeviceDoc/22288A.pdf)
 * 			- Désactiver le mouvement du curseur de la souris par le joystick ()
 */

/* Librairies  */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/input.h>
#include <linux/types.h>
#include <linux/joystick.h>
//#include "hidapi.h"

/* Librairies personelles */
#include "header/map.h"
#include "header/nrf24l01.h"
#include "header/hid.h"


char *axis_names[] = {
"X", "Y", "", "Rx", "Ry", "" 
};

char *button_names[] = {
"X", "Rond", "Triangle", "Carre", "L1", "R1", "L2", "R2", "Select", "Start", "PS", "JG", "JD", "Haut", "Bas", "Gauche",
"Droit"
};

#define NAME_LENGTH 128		// Nombre de caractère maximum du nom

int main (int argc, char **argv)
{
	int fd_joystick, fd_mcp2210, i, req;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000001;
	char name[NAME_LENGTH] = "Inconnu";
	unsigned char buffer[65];
	unsigned char data[33];
	unsigned char *mcp2210 = "/dev/mcp2210";

	/* Variable d'affichage */
	unsigned char battery = 0;
	float speed = 0;
	float angle = 0;
	unsigned char balle = 0;
	/* Reset du buffer (valeur 0x00 pour chaque octet */
	memset(buffer, 0x0, 65);
	memset(data, 0x0, 33);

	uint16_t btnmap[BTNMAP_SIZE];
	uint8_t axmap[AXMAP_SIZE];
	int btnmapok = 1;

	if (argc == 3)
		mcp2210 = argv[2];


	if (argc < 3 || argc > 4 || !strcmp("--help", argv[1])) {
		/* Aide -- Si le bon nombre d'argument n'est pas donné au programme  */
		puts("");
		puts("Utilisation: ./contoller [<mode>] <joystick> <device_mcp2210>");
		puts("");
		puts("Modes:");
		puts("  --test");
		puts("  --event            Affiche les évenements au fur et à mesure");
		return 1;		// Code erreur pour le débug
	}

	/* On vérifie que la manette est connectée 
	 * n cas d'erreur la fonction open renvoit -1
	 */
	if ((fd_joystick = open(argv[argc - 2], O_RDONLY)) == -1) {
		perror("Manette PS3/PS4(?)");
	}
    /**
     * initialise le converteur USB-SPI MCP2210.
	 * En cas d'erreur la fonction open renvoit -1
     */
	if ((fd_mcp2210 = open(argv[argc-1], O_RDWR)) == -1){
		perror("MCP2210-I/SO");
	}
	/* Gestion du périphérique
	 * Erreur : renvoit 0 si réussi, -1 pour erreur (ioctl)
	 * errno : EBADF -> mauvais descripteur
	 * 	   EFAULT -> pointe hors de l'espace d'adressage valide
		   EINVAL -> requête ou argument non valide
		   ENOTTY -> le chemin donnée (fd_joystick) ne correspond pas à un fichier spécial (fichier "périphérique")
		   	 ou  la requête ne s'applique pas au type d'objet associé au fd_joystick
	 */

	ioctl(fd_joystick, JSIOCGVERSION, &version);
	ioctl(fd_joystick, JSIOCGAXES, &axes);
	ioctl(fd_joystick, JSIOCGBUTTONS, &buttons);
	ioctl(fd_joystick, JSIOCGNAME(NAME_LENGTH), name);

	get_joystick(fd_joystick, axmap);		// getaxmap équivaut à un ioctl paramétré pour les joystics
	get_button(fd_joystick, btnmap);		// getbtnmap équivaut à un ioctl paramétré pour les boutons

	/* Fin gestion des périphériques */


	printf("Version du driver  %d.\n",version);

	/* Determine si les boutons définit dans le tableau sont disponible. */
	for (i = 0; btnmapok && i < buttons; i++) {
		if (btnmap[i] < BTN_MISC || btnmap[i] > KEY_MAX) {
			btnmapok = 0;
			break;
		}
	}
	if (!btnmapok) {
		/* btnmap ne contient pas le bon nombre de bouton. Impossible d'afficher les noms des boutons */
		puts("Le programme n'est pas entièrement compatible avec votre kernel. Impossible de charger la disposition des boutons!");
		printf("La manette (%s) a %d joysticks ", name, axes);
		printf("et %d boutons.\n", buttons);
	} else {
		printf("La manette (%s) a %d axe(s) (", name, axes);
		for (i = 0; i < axes; i++)
			printf("%s%s", i > 0 ? ", " : "", axis_names[axmap[i]]);
		puts(")");

		printf("et %d boutons (", buttons);
		for (i = 0; i < buttons; i++) {
			printf("%s%s", i > 0 ? ", " : "", button_names[btnmap[i] - BTN_MISC]);
		}
		puts(").");
	}

	printf("Test en cours... (ctrl+c ou boutton PS pour fermer le programme)\n");

/*
 *	Initialisation RF
 */
Init_RF(fd_mcp2210, buffer);
conf_Tx_Addr(fd_mcp2210, buffer);
conf_Rx_Addr(fd_mcp2210, buffer);

// Initialisation GPIO: CE = 1, SS = 1
set_gpio(fd_mcp2210, buffer, 1,1);
/* 
 * Mode par defaut 
 */

	if (argc >= 2 || !strcmp("--normal", argv[1])) {

		int *axis;
		char *button;
		int i;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			printf("\x1b[H");
			/* Test de lecture du fichier joystick */
			if (read(fd_joystick, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\nManette Playstation 3 (4?): erreur de lecture.");
				return 1;
			}

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}
#ifdef DEBUG
			printf("\r");

			if (axes) {
				printf("Joystick: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}

			if (buttons) {
				printf("Boutons: ");
				for (i = 0; i < buttons; i++)
					printf("%s:%s ", button_names[i], button[i] ? "on " : "off");
					printf("%s", button[i]);
			}
#endif
/*
 * Affichage
 * On récupère les informations:
 * 0x00 - Batterie (char)
 * 0x01 - Vitesse (float?)
 * 0x02 - Angles (float?)
 * 0x03 - présence de la balle (char 1 = présence de la balle)
 */
			Rx_On(fd_mcp2210, buffer);
			if(Rx_receive(fd_mcp2210, buffer)){ // Si on a reçu des données, on mets à jour les variables qui s'afficheront
				switch(buffer[4]){
					case 0x00:
						battery = (buffer[5]/0x64)*0xFF; //Batterie en pourcentage
						break;
					case 0x01:
						for (i=5; i < 9; i++){
							speed = (buffer[i] >> 8) & 0xff;
						}
						break;
					case 0x02:
						for (i=5; i < 9; i++){
							angle = (buffer[i] >> 8) & 0xff;
						}
						break;
					case 0x03:
						balle = buffer[5];
				}
			}
// Affichage
			print_spi_settings(fd_mcp2210);
			printf("|========| Robot Soccer |=========================================|\n");
			printf("|=================================================================|\n");
			printf("Batterie (pourcentage) : %hhx \n\t",battery);
			printf("Vitesse du robot: %.2f m/s\n\t",speed);
			printf("Angles : %.2f °\n\t",angle);
			printf("La balle est ");
			printf("%s", balle ? "indisponible\n" : "visible\n");
			printf("|=================================================================|\n");

			data[0] = W_TX_PAYLOAD;
			data[1] = 0x01; // Code mouvement
			ConvertShortToByteArray(axis[0], data, 2); // Joystick gauche
			ConvertShortToByteArray(axis[1], data, 4); // Joystick droit
			if (button[5] > 0)
			{
				data[5] = 0xFF; // Bouton R1 appuyé
			} else {
				data[5] = 0x00; // Bouton non appuyé
			}
			set_gpio(fd_mcp2210, buffer,1,1); // CE = 0 SS = 1
			// Passage en mode Tx
			set_gpio(fd_mcp2210, buffer, 0, 1);
			set_gpio(fd_mcp2210, buffer, 0, 0);
			configuration_reg(fd_mcp2210, buffer, W_REGISTER,V_CONFIG_ON_TX);
			set_gpio(fd_mcp2210, buffer, 0, 1);
			set_gpio(fd_mcp2210, buffer, 0, 0);
			send_spi(fd_mcp2210, buffer, data, 6); // On envoit les données du joystick
			set_gpio(fd_mcp2210, buffer, 0,1); // CE = 0, SS = 1
			set_gpio(fd_mcp2210, buffer, 1, 1);	// CE = 1, SS = 1
			
			printf("\r");
			printf("Joystick: ");
			printf("%2d:%6d ", 0, axis[0]);
			printf("%2d:%6d ", 1, axis[1]);
			printf("%2d:%6d ", 3, axis[3]);
			printf("%2d:%6d ", 4, axis[4]);
			printf("Boutons: ");
			printf("%s:%s ", button_names[5], button[5] ? "on " : "off");

			/*
			 * Librairie: stdio.h
			 * Prototype: int fflush(FILE *stream);
			 * Vide l'espace tampon (buffer) d'un flux 
			 */
			printf("\x1b[J");
			fflush(stdout);
			printf("\033c" );
		}
	}
}
