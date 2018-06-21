/*
 * RF.c
 *
 *  Created on: Mar 24, 2018
 *      Author: ybrau
 */

#include "RF.h"
#include <string.h>
extern T_Buffer transmite;
extern T_Buffer receive;
extern unsigned char Status_check;

void Init_RF(void)
{
	SPI_Init_On();
	PTBDD_PTBDD5 = 0;	// Input (output driver disabled) for port B bit 0 (pin IRQ)
	PTEDD_PTEDD5 = 1;	// Output driver enabled for port B bit 1 (pin CE)
	CE = 0;
	SS = 1;
	configuration_reg(W_RF_CH,V_RF_CH);	// Set wave frequency: 2.43GHz
	configuration_reg(W_RX_PW_P0,0x00);
	configuration_reg(W_RX_PW_P1,0x00);
	configuration_reg(W_RX_PW_P2,0x00);
	configuration_reg(W_RX_PW_P3,0x00);
	configuration_reg(W_RX_PW_P4,0x00);
	configuration_reg(W_RX_PW_P5,0x00);
	configuration_reg(W_RF_SETUP,V_RF_SETUP);
	configuration_reg(W_CONFIG,V_CONFIG_OFF);
	configuration_reg(W_EN_AA,V_EN_AA);
	configuration_reg(W_EN_RXADDR, V_EN_RXADDR);
	configuration_reg(W_SETUP_RETR,V_SETUP_RETR);
	configuration_reg(W_SETUP_AW,V_SETUP_AW);
	Clean_RX();
	/* Configuration terminé. On allume le module radio */
	configuration_reg(W_STATUS,0x70); // Vide le status register
	configuration_reg(W_REGISTER,V_CONFIG_ON_TX);
}

void configuration_reg(unsigned char addr, unsigned char data)
{
	clear_buffer(&transmite);
	transmite.buffer[0] = addr;
	transmite.buffer[1] = data;
	transmite.n = 2;
	SS = 0;
	SPI_Send(&transmite,2);
}
/*
 * Configuration des adresses des canaux de transmission (Tx) et émission (Rx)
 * pour changer d'adresse modifiez le fichier RF.h
 */
void configuration_Tx(void)
{
	unsigned char temp, i;
	 SS = 0;
	
	 // Remplissage du buffer d'envoi avant paramétrage de l'adresse de réception du module RF
	 
	transmite.buffer[0] = W_RX_ADDR_P0;
	SS = 1;
	temp = 0xE7; // V_ADDR_TX
	transmite.n = 6;
	for(i = 1; i < 6;++i)
	{
		transmite.buffer[i] = temp;
	}
	SPI_Send(&transmite,6);
	SS = 0;
	/*
	 * Remplissage du buffer d'envoi avant paramétrage de l'adresse d'émission 
	 */
	 SS = 1;
	 transmite.buffer[0] = W_TX_ADDR;
	for(i = 1; i < 6;++i)
	{
		transmite.buffer[i] = temp;
	}
	SPI_Send(&transmite,6);
	SS = 1;
}
void configuration_Rx(void)
{
	unsigned char i,temp;
	CE = 0;
	SS = 0;
	/* Le premier octet correspond à la commande de modification d'adresse du canal de réception */
	transmite.buffer[0] = W_RX_ADDR_P1;
	temp = 0xA2; // 0xA2A2A2A2A2 = Addresse du payload 1
	/*
	 * Remplissage du buffer d'envoi avant paramétrage de l'adresse d'émission
	 */
	for(i = 1; i < 6;++i)
	{
	// Cast volontaire afin de récupérer l'octet suivant
		transmite.buffer[i] = temp;
	}
	SPI_Send(&transmite,6);
	SS = 1;
	CE = 1;
}

/*
 * Appels status
 * Retourne 1 en cas de succès, 0 si une trame est en cours d'envoi
 */
void Rx_check(void)
{
	unsigned char status_reg = 0;
	/*
	 * Vérifie qu'aucune trame n'est en cours d'envoi
	 */
	if(transmite.n == 0)
	{
		transmite.buffer[0] = 0xFF; // NOP pour récupérer le status
		SS = 0;
		SPI_Send(&transmite,1);
		Status_check = 0x01; /* 0x00 = Ignorer la donnée en reception (retour status register automatique); 0x01 = données à récupérer*/
		SS = 1;
	}
}

void Tx_Send(unsigned char *tab, unsigned char n)
{
	unsigned char i = 0;
	CE = 0;
	configuration_reg(W_CONFIG,V_CONFIG_ON_TX);
	transmite.n = n++;
	SS = 0;
	transmite.buffer[0] = W_TX_PAYLOAD;
	for(i=0;i < n;++i){
		transmite.buffer[i+1] = tab[i];   
		i++;
	}
	SPI_Send(&transmite,transmite.n);
}
void Clean_RX(void)
{
	SS = 0;
	clear_buffer(&transmite);
	transmite.buffer[0] = FLUSH_RX;
	SPI_Send(&transmite,1);
	SS = 1;
}

void clear_buffer(struct BufferC* buffer)
{
	//memset_clear_8bitCount(buffer->buffer,sizeof(buffer->buffer));	// Réinitialise le contenu du buffer
	unsigned char i;
	buffer->p = buffer->buffer;										// Réinitialise le pointeur
	buffer->n = SizeBuffer;											// Réinitialise le compteur d'octet
	for(i=0;i< SizeBuffer;++i){
		buffer->buffer[i] = 0x00;
	}
	return;
}

void Sort_Bytes(T_Buffer *buffer)
{
	switch(buffer->buffer[0]){
		case 0x01:	// Code pour modification du mouvement du véhicule
			switch(buffer->buffer[1]){
				case 0x01: // Axe 1
					break;
				case 0x02:
					break;
			}
			break;
		case 0x02:	// Code pour modification du mouvement du véhicule
			break;
	}
}
