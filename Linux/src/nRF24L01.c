#include "header/nrf24l01.h"
#include "header/hid.h"
unsigned char V_ADDR_RX[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
unsigned char V_ADDR_TX[5] = {0xA2,0xA2,0xA2,0xA2,0xA2};

void Init_RF(int fd, unsigned char *buffer)
{
    set_gpio(fd, buffer, 1, 1);
    set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RF_CH,V_RF_CH);	// Set wave frequency: 2.43GHz
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P0,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P1,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P2,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P3,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P4,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RX_PW_P5,0x00);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_RF_SETUP,V_RF_SETUP);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_CONFIG,V_CONFIG_OFF);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_EN_AA,V_EN_AA);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_EN_RXADDR, V_EN_RXADDR);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_SETUP_RETR,V_SETUP_RETR);
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	configuration_reg(fd, buffer, W_SETUP_AW,V_SETUP_AW);
	set_gpio(fd, buffer, 0, 1);
	Rx_On(fd, buffer);
}

void configuration_reg(int fd, unsigned char *buffer, unsigned char registre, unsigned char value)
{
    unsigned char data[32];
    data[0] = registre;
    data[1] = value;
    memset(data, 0x0, 32);
    send_spi(fd, buffer, data, 2);
}

void Clean_RX(int fd, unsigned char *buffer)
{
    unsigned char data[33];
    set_gpio(fd, buffer, 0, 0);
    data[0] = FLUSH_RX;
    send_spi(fd, buffer, data, 1);
    while(buffer[1] != 0x00)
    {
        read(fd, buffer, 64);
    }
	set_gpio(fd, buffer, 0, 1);
}

void Rx_On(int fd, unsigned char *buffer)
{
	Clean_RX(fd, buffer);
	/* Configuration terminé. On allume le module radio */
	configuration_reg(fd, buffer, W_STATUS,0x70); // Vide le status register
	set_gpio(fd, buffer, 0, 1);
	configuration_reg(fd, buffer, W_REGISTER,V_CONFIG_ON_TX);
	set_gpio(fd, buffer, 1, 1);
}

void conf_Tx_Addr(int fd, unsigned char *buffer)
{
	unsigned char data[32];
	unsigned char i;
	set_gpio(fd, buffer, 1, 0);
	data[0] = W_RX_ADDR_P0;
	for (i = 1; i < 6; i++){
		data[i] = V_ADDR_TX[i-1];
	}
	send_spi(fd, buffer, data, 6);
	set_gpio(fd, buffer, 1, 1);
	
	set_gpio(fd, buffer, 1, 0);
	data[0] = W_RX_ADDR_P1;
	for (i = 1; i < 6; i++){
		data[i] = V_ADDR_TX[i-1];
	}
	send_spi(fd, buffer, data, 6);
	set_gpio(fd, buffer, 1, 1);
}

void conf_Rx_Addr(int fd, unsigned char *buffer)
{
	unsigned char data[33];
	unsigned char i;
	set_gpio(fd, buffer, 0, 0);
	data[0] = W_RX_ADDR_P1;
	for (i = 1; i < 6; i++){
		data[i] = V_ADDR_TX[i-1];
	}
	send_spi(fd, buffer, data, 6);
	set_gpio(fd, buffer, 1, 1);
}

unsigned char Rx_receive(int fd, unsigned char *buffer){
	unsigned char data[32];
	// envoi NOP pour récupérer le statut du status register
	memset(buffer, 0x0, 64); // Remise à zéro du buffer
	memset(data, 0x0, 32); // Remise à zéro du buffer
	set_gpio(fd, buffer, 0, 1);
	set_gpio(fd, buffer, 0, 0);
	if (send_spi(fd, buffer, data, 1) == 0){ 
		if ((buffer[4]&0x40) != 0x40) { // Si des données sont disponible on les récupères
			set_gpio(fd, buffer, 0, 1);
			set_gpio(fd, buffer, 0, 0);
			data[0] = R_RX_PAYLOAD;	// Envoi la commande pour récupéré le buffer de réception
			if (send_spi(fd, buffer, data, 1) == 0){
				return 1;
			}
			
		}
	}
	set_gpio(fd, buffer, 0, 1);
	return 0;
}