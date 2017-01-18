/*
 * GPS.c
 *
 *  Created on: Aug 4, 2016
 *      Author: Rafi
 */


#include "sensors.h"

void GPS_on(void)
{
	GPIOPinWrite(GPIOA1_BASE, GPIO_PIN_1, 0x00); //Turn on GPS
}

void GPS_off(void)
{
	GPIOPinWrite(GPIOA1_BASE, GPIO_PIN_1, 0xFF); //Turn off GPS
}


build_gps_ubx_message{
	if (get)
		switch (msg_name)
			case CFG_MSG :
			case NAV_MSG :
	else if (set)
		switch (msg_name)
			case CFG_MSG :
			case NAV_MSG :
}

void build_gps_ubx_msg(char msg_class, char msg_id, int msg_len, char* payload){
	char ck_a, ck_b;
	int msg_tot_len	 = MSG_HDR_LEN + MSG_CLASS_LEN + MSG_ID_LEN+ MSG_LEN_IND + pay_len + CKS_LEN;
	int msg_body_len = MSG_CLASS_LEN + MSG_ID_LEN+ MSG_LEN_IND + pay_len;
	int ck_a_pos = msg_tot_len -2;
	int ck_a_pos = msg_tot_len -1;
	char msg_len_low  = msg_len;
	char msg_len_high = msg_len >> BYTE;
	char msg_body [msg_body_len] = {msg_class, msg_id, msg_len_low, msg_len_high};
	memcpy ((msg_body+4*sizeof(char)), payload, strlen(payload));
	calc_msg_checksum (&ck_a, &ck_b, msg_body,msg_body_len);
	char msg[msg_tot_len] = {SYNC_CHAR1,SYNC_CHAR2,msg_class,msg_id,msg_len_low,msg_len_high};
	memcpy ((msg+6*sizeof(char)),payload,strlen(payload));
	msg[msg_tot_len-2] =ck_a;
	msg[msg_tot_len-1] =ck_b;
}

void calc_msg_checksum (int* CK_A, int* CK_B, char* msg_body, int msg_body_len){
	int byte_num, bit_num;
	int current_byte, current_bit;
	for(byte_num=0; byte_num<msg_body_len; byte_num++){
		current_byte = msg_body+sizeof(char)*byte_num
		for (bit_num=0; bit_num<BYTE; bit_num++){
			*CK_A = *CK_A + (current_byte & 0x1);
			*CK_B = *CK_B + *CK_A;
			current_byte = current_byte << BIT;
		}
	}
}

