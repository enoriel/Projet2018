// SPDX-License-Identifier: GPL-2.0
/*
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include "header/hid.h"

const char *bus_str(int bus);

#ifdef DEBUG
int main(int argc, char **argv)
{
	int fd;
	int i, res, desc_size = 0;
	unsigned char buffer[65];
	unsigned char data[32];
	data[0] = 0x32;
	data[1] = 0x33;
	data[2] = 0x32;
	data[3] = 0x33;
	char *device = "/dev/mcp2210";
	/* Reset du buffer (valeur 0x00 pour chaque octet */
	memset(buffer, 0x0, 65);
	memset(data, 0x0, 32);
//	p = buffer;
	if (argc == 1)
		device = argv[0];

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open(device, O_RDWR);

	if (fd < 0) {
		perror("Impossible de se connecter à la carte.");
		return 1;
	}
	print_mcp2210_info(fd);
	print_spi_settings(fd);
	get_gpio(fd, buffer);
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
	set_gpio(fd, buffer, 1, 1);
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
	send_spi(fd, buffer, data, sizeof(data));
	printf("\n");
	return 0;
}
#endif

unsigned char* ConvertShortToByteArray(short int value, unsigned char *buffer, char n)
{
	buffer[n] = (value >> 8) & 0xff; // High Byte
	buffer[n+1] = value & 0xff; // Low Byte
	return buffer;
}

unsigned char send_spi(int fd, unsigned char *buffer, unsigned char *data, char n)
{
	unsigned char i = 0;
	memset(buffer, 0x0, 65); // Remise à zéro du buffer
	buffer[0] = 0x42; // Code commande transfert SPI
	buffer[1] = n;	// Nombre d'octet à envoyer
	for(i=0; i < n; i++)
	{
		buffer[4+i] = data[i]; 
	}
	buffer = send_data(fd, buffer);
#ifdef DEBUG
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
	if (buffer[1] == 0xF7 || buffer[1] == 0xF8)
		printf("Le transfert à échoué!");
	if (buffer[1] ==  0x00)
		printf("Transfert réussi!");
#endif
	/* Vérifie si des données SPI on était reçu. */
	if (buffer[2] > 0){ // retourne 0 si on a reçu des données
		return 0;
	}
	else{
		return 1;
	}
}

unsigned char* set_gpio(int fd, unsigned char *buffer, int CE, int SS)
{
	unsigned i = 0;
	memset(buffer, 0x0, 65); // Remise à zéro du buffer
	buffer[0] = 0x30; // Code commande SET value GPIO
	if (CE == 1 && SS == 1)
		buffer[4] = 0x03;
	if (CE == 1 && SS == 0)
		buffer[4] = 0x02;
	if (SS == 1 && CE == 0)
		buffer[4] = 0x01;
#ifdef DEBUG
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
#endif
	buffer = send_data(fd, buffer);
#ifdef DEBUG
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
#endif
	return buffer;
}

unsigned char* get_gpio(int fd, unsigned char *buffer)
{
	unsigned i = 0;
	memset(buffer, 0x0, 65);
	buffer[0] = 0x31; // Code commande GET value  GPIO
#ifdef DEBUG
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n");
#endif
	buffer = send_data(fd, buffer);
#ifdef DEBUG
	for(i=0;i < 64;i++){
		printf("%hhx ",buffer[i]);
	}
	printf("\n\t");
#endif
	return buffer;
}

unsigned char* send_data(int fd, unsigned char *buffer)
{
	int i,res = 0;
	res = write(fd, buffer, 64);
	if (res <0) {
		printf("Erreur lors de la communication des données.\n\t");
	} else {
		res = read(fd, buffer, 64);
		if (res < 0){
		printf("Erreur lors de la récupération de la réponse.\n\t");
		}
#ifdef DEBUG
		for(i=0;i < 64;i++){
			printf("%hhx ",buffer[i]);
		}
		printf("\n");
#endif
	}

	return buffer;
}

struct spi_settings print_spi_settings(int fd)
{
	int i, res = 0;
	struct spi_settings value;

	/* Send a Report to the Device */
	value.SPI_SETB[0] = 0x41; /* Octet d'état du registre de configuration SPI (mémoire volatile)*/
	value.SPI_SETB[1] = 0x00; /* Power up chip setting */
	for(i=2; i < 64; i++){
		value.SPI_SETB[i] = 0x00;
	}

	/* Get a report from the device */
	res = write(fd, value.SPI_SETB, 64);
		if (res < 0) {
			printf("Erreur: %d\n", errno);
#ifdef DEBUG
			perror("write");
#endif
		} else {
			//printf("write() a écrit %d octets\n\t", res);
		}
	res = read(fd, value.SPI_SETB, 64);
	if (res < 0) {
		perror("read");
	} else {
		//printf("read() a récupéré %d octets:\n\t", res); /* Affichage pour débug */
		//printf("|=================================================================\n");
		printf(" _________________________________________________________________\n");
		printf("|========| Configuration SPI de la carte USB-SPI |================|\n");
		printf("|=================================================================|\n");
		printf("|              Code commande : %hhx                                 |\n", value.SPI_SETB[0]);
		printf("|              Résultat de la requête : ");
		if(value.SPI_SETB[1] == 0x00)
			printf("transfert réussi!         |\n");
		else
			printf("erreur lors du transfert?!|\n");

		printf("|              Bit rate : %hhx%hhx%hhx%hhx                                 |\n", value.SPI_SETB[4], value.SPI_SETB[5], value.SPI_SETB[6], value.SPI_SETB[7]);
		printf("|              Mode SPI : %x                                       |\n",value.SPI_SETB[7]);
		printf("|==================================================================\n");
	}

	return value;
}

void print_mcp2210_info(int fd){
	const char *bus_str(int bus);
	int i, res, desc_size = 0;
	unsigned char buf[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
//	memset(buf, 0x0, sizeof(buf));

	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	/* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);

	/* Get Raw Info */
	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else {
		printf("Raw Info:\n");
//		printf("\tbustype: %d (%s)\n",
//			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}
}