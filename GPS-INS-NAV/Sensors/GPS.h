/*
 * GPS.h
 *
 *  Created on: Aug 4, 2016
 *      Author: Rafi
 */

#ifndef GPS_H_
#define GPS_H_


void GPS_on(void);
void GPS_off(void);

#define BYTE 8
#define BIT 1
//MSG structure
#define SYNC_CHAR1 		0xB5
#define SYNC_CHAR2 		0x62
#define MSG_HDR 		((SYNC_CHAR1 << 8) & SYNC_CHAR1)
#define MSG_HDR_LEN 	0x02
#define MSG_CLASS_LEN 	0x01
#define MSG_ID_LEN 		0x01
#define MSG_LEN_IND 	0x02
#define MSG_PAYLOAD_LEN
#define MSG_CKS_LEN		0x02



//classes defines
#define CFG_MSG_CLASS 0x06
#define NAV_MSG_CLASS 0x01

//ID defines
#define CFG_MSG_ID 0x01
#define CFG_PRT_ID 0x00

//typical msg lengths
#define CFG_MSG_LEN 20



#endif /* GPS_H_ */