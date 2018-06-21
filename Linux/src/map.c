/*
 * Support des joysticks et bouttons.
 * Enoriel  <yassine.elmernissi@ynov.com>
 */
#ifndef _MAP_H_
#define _MAP_H_

#include <errno.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/input.h>
#include <linux/joystick.h>

#include "header/map.h"

/* Les valeurs suivantes viennent des fichiers include/joystick.h des sources kernel. */
#define JSIOCSBTNMAP_LARGE _IOW('j', 0x33, __u16[KEY_MAX_LARGE - BTN_MISC + 1])
#define JSIOCSBTNMAP_SMALL _IOW('j', 0x33, __u16[KEY_MAX_SMALL - BTN_MISC + 1])
#define JSIOCGBTNMAP_LARGE _IOR('j', 0x34, __u16[KEY_MAX_LARGE - BTN_MISC + 1])
#define JSIOCGBTNMAP_SMALL _IOR('j', 0x34, __u16[KEY_MAX_SMALL - BTN_MISC + 1])

/*
 * Détermine le paramètre ioctl nécéssaire
*/
int find_ioctl(int fd, int *ioctls, int *ioctl_used, void *argp)
{
	int i, retval = 0;

	/* On test tous les paramètres ioctl possible */
	for (i = 0; ioctls[i]; i++) {
		if ((retval = ioctl(fd, ioctls[i], argp)) >= 0) {
			/* Paramètre ioctl valide. */
			*ioctl_used = ioctls[i];
			return retval;
		} else if (errno != -EINVAL) {
			/* errno est vrai donc une erreur à eux lieu, on passe au paramètre suivant. */
			return retval;
		}
	}
	return retval;
}

int get_button(int fd, uint16_t *btnmap)
{
	static int jsiocgbtnmap = 0;
	int ioctls[] = { JSIOCGBTNMAP, JSIOCGBTNMAP_LARGE, JSIOCGBTNMAP_SMALL, 0 };

	if (jsiocgbtnmap != 0) {
		/* Le paramètre ioctl valide est déjà connu */
		return ioctl(fd, jsiocgbtnmap, btnmap);
	} else {
		/* on selectionne le bon paramètre ioctl */
		return find_ioctl(fd, ioctls, &jsiocgbtnmap, btnmap);
	}
}

int set_button(int fd, uint16_t *btnmap)
{
	static int jsiocsbtnmap = 0;
	int ioctls[] = { JSIOCSBTNMAP, JSIOCSBTNMAP_LARGE, JSIOCSBTNMAP_SMALL, 0 };

	if (jsiocsbtnmap != 0) {
		/* Le paramètre ioctl valide est déjà selectionné. */
		return ioctl(fd, jsiocsbtnmap, btnmap);
	} else {
		/* Selectionne le bon paramètre ioctl */
		return find_ioctl(fd, ioctls, &jsiocsbtnmap, btnmap);
	}
}

int get_joystick(int fd, uint8_t *axmap)
{
	return ioctl(fd, JSIOCGAXMAP, axmap);
}

int set_joystick(int fd, uint8_t *axmap)
{
	return ioctl(fd, JSIOCSAXMAP, axmap);
}

#endif