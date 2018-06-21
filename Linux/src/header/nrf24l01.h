#ifndef _NRF24L01_H_
#define _NRF24L01_H_

/* Commands */
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0x50
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define R_RX_PL_WID 0x60
#define W_ACK_PAYLOAD 0xA8
#define W_TX_PAYLOAD_NOACK 0xB0
#define NOP 0xFF

/* Registers */
#define R_CONFIG 0x00
#define W_CONFIG 0x20
#define R_EN_AA 0x01
#define W_EN_AA 0x21
#define R_EN_RXADDR 0x02
#define W_EN_RXADDR 0x22
#define R_SETUP_AW 0x03
#define W_SETUP_AW 0x23
#define R_SETUP_RETR 0x04
#define W_SETUP_RETR 0x04
#define R_RF_CH 0x05
#define W_RF_CH 0x25
#define R_RF_SETUP 0x06
#define W_RF_SETUP 0x06
#define R_STATUS 0x07
#define W_STATUS 0x07
#define R_OBSERVE_TX 0x08
#define W_OBSERVE_TX 0x28
#define R_RPD 0x09
#define W_RPD 0x29
#define R_RX_ADDR_P0 0x0A
#define W_RX_ADDR_P0 0x2A
#define R_RX_ADDR_P1 0x0B
#define W_RX_ADDR_P1 0x2B
#define R_RX_ADDR_P2 0x0C
#define W_RX_ADDR_P2 0x2C
#define R_RX_ADDR_P3 0x0D
#define W_RX_ADDR_P3 0x2D
#define R_RX_ADDR_P4 0x0E
#define W_RX_ADDR_P4 0x2E
#define R_RX_ADDR_P5 0x0F
#define W_RX_ADDR_P5 0x2F
#define R_TX_ADDR 0x10
#define W_TX_ADDR 0x30
#define W_RX ADDR W_TX_ADDR
#define R_RX_PW_P0 0x11
#define W_RX_PW_P0 0x31
#define R_RX_PW_P1 0x12
#define W_RX_PW_P1 0x32
#define R_RX_PW_P2 0x13
#define W_RX_PW_P2 0x33
#define R_RX_PW_P3 0x14
#define W_RX_PW_P3 0x34
#define R_RX_PW_P4 0x15
#define W_RX_PW_P4 0x35
#define R_RX_PW_P5 0x16
#define W_RX_PW_P5 0x16
#define FIFO_STATUS 0x17
#define DYNPD 0x1C
#define R_FEATURE 0x1D
#define W_FEATURE 0x3D

/* Configuration values */
#define V_CONFIG_ON_TX 0x7A // MASK_RX_DR = 1(disable IRQ Pin), MASK_TX_DS = 1(disable IRQ Pin), MASK_MAX_RT = 1 (disable IRQ Pin), EN_CRC = 1 (enable CRC), CRC0 = 1 (CRC 2 bytes), PWR_UP = 1 (Power up RF mod), PRIM_RX = 0 (Tx enable)
#define V_CONFIG_ON_RX 0x7B // MASK_RX_DR = 1(disable IRQ Pin), MASK_TX_DS = 1(disable IRQ Pin), MASK_MAX_RT = 1 (disable IRQ Pin), EN_CRC = 1 (enable CRC), CRC0 = 1 (CRC 2 bytes), PWR_UP = 1 (Power up RF mod), PRIM_RX = 1 (Rx enable)
#define V_CONFIG_OFF 0x7C // MASK_RX_DR = 1(disable IRQ Pin), MASK_TX_DS = 1(disable IRQ Pin), MASK_MAX_RT = 1 (disable IRQ Pin), EN_CRC = 1 (enable CRC), CRC0 = 1 (CRC 2 bytes), PWR_UP = 0 (Power up RF mod), PRIM_RX = 0 (Tx enable)
#define V_EN_AA 0x3F // RESERVED[7:6] = 00 (default value), ENAA_P[5:0] = 111111 enable auto-ack
#define V_EN_RXADDR 0x03 // RESERVED[7:6] = 00 (default value), ERX_P[5:0] = 000011 (enable data pipe 0 et 1)
#define V_SETUP_AW 0x01 // ARD[7:4] = 0000 (automatic retransmission enable - ARD = 250µs), ARC[3:0] = 0001 (retransmit once on fail)
#define V_RF_CH 0x1E // RESERVED = 0 (default value), RF_CH[6:0] = 0011110 (Sets the frequency) ; F0 = 2400 + RF_CH MHz = 2430 MHz = 2,43GHz
#define V_RF_SETUP 0x10 // CONT_WAVE = 0, RF_DR_LOW = 0 & RF_DR_HIGH = 1 (set speed data rate to 2Mbps - maximum speed), RF_PWR = 00 (set power to -18dBm, lowest consumption level)
//#define V_RX_ADDR_P0 0xE7E7E7E7E7
//#define V_TX_ADDR 0xE7E7E7E7E7
#define V_RX_PW_P0 0x04 // 4 bits buffer RX
#define V_DYNPD 0x05 // enable dynamic payload lentgth on data pipe 0
#define V_FEATURE 0x03
#define V_SETUP_RETR 0x12

/* Prototypes */
void Init_RF(int fd, unsigned char *buffer);
void Clean_RX(int fd, unsigned char *buffer);
void Rx_On(int fd, unsigned char *buffer);
void configuration_reg(int fd, unsigned char *buffer, unsigned char registre, unsigned char value);
void conf_Tx_Addr(int fd, unsigned char *buffer);
void conf_Rx_Addr(int fd, unsigned char *buffer);
unsigned char Rx_receive(int fd, unsigned char *buffer);

#endif