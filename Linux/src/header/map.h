/*
 * Gestion de la manette de Playstation 3 
 * Langage C
 * Par Enoriel <"yassine.elmernissi@ynov.com">
 * Version 0.1
 * --------------------- Changelog -----------
 * -- Version 0.1:
 * Fonctionnalité de lecture de base + affichage des données envoyé par les joysticks et bouttons
 */

#ifndef __MAP_H__
#define __MAP_H__

#include <stdint.h>
#include <linux/input.h>

/* Les valeurs suivantes viennent du kernel (include/input.h)
 */
#define KEY_MAX_LARGE 0x2FF
#define KEY_MAX_SMALL 0x1FF
/* Taille maximum du champs joystick */
#define AXMAP_SIZE (ABS_MAX + 1)

/* Taille max du champs boutton  */
#define BTNMAP_SIZE (KEY_MAX_LARGE - BTN_MISC + 1)

/* Prototype */
int get_joystick(int fd, uint8_t *axmap);
int set_joystick(int fd, uint8_t *axmap);
int get_button(int fd, uint16_t *btnmap);
int set_button(int fd, uint16_t *btnmap);

#endif
