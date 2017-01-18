//Magnetometer
/*
 * HMC5883.c
 *
 *  Created on: Jun 6, 2016
 *      Author: Rafi
 */

#include "sensors.h"

SHORT Xmcal=400, Ymcal=400, Zmcal=400;
float Xmoff=0, Ymoff=0, Zmoff=0;
float Xmax=0, Ymax=0, Zmax=0;
float Xmin=0, Ymin=0, Zmin=0;


int init_HMC5883(char addr, char cal)
{
	unsigned char data[5];
	unsigned char rdata[10];

	if(cal)
	{
		data[0] =  0x00;
		data[1] =  0x71;
		data[2] =  0xA0;//0xA0
		data[3] =  0x00;
		if(I2C_IF_Write(addr, data, 4, true) < 0) 	return -1;
		UtilsDelay(2000000); //75ms delay
		while(I2CMasterBusy(I2CA0_BASE)){}

		data[0] = 0x00;
		if(I2C_IF_Write(addr, data, 1, false)<0)	return -2;
		while(I2CMasterBusy(I2CA0_BASE)){}
		if(I2C_IF_Read(addr, rdata, 9)<0)			return -3;
		while(I2CMasterBusy(I2CA0_BASE)){}

		Xmcal = (int)rdata[3];
		Xmcal <<=8;
		Xmcal += (int)rdata[4];

		Zmcal = (int)rdata[5];
		Zmcal <<=8;
		Zmcal += (int)rdata[6];

		Ymcal = (int)rdata[7];
		Ymcal <<=8;
		Ymcal += (int)rdata[8];
	}
	else
	{
		Xmcal=400;
		Ymcal=400;
		Zmcal=400;

	}

	data[0] =  0x00;
	data[1] =  0x70;
	data[2] =  0xA0;//A0
	data[3] =  0x00;
	if(I2C_IF_Write(addr, data, 4, true) < 0)	return -4;
	while(I2CMasterBusy(I2CA0_BASE)){}
	UtilsDelay(2000000); //75ms delay

	return 0;
}



int HMC5883_read_magdata(char addr, Mag_inner_data_str* mag_data )
{
	unsigned char data[2];
	unsigned char rdata[10];
	SHORT Xdata, Ydata, Zdata;

	data[0] = 0x00;
	if(I2C_IF_Write(addr, data, 1, false)<0)	return -1;
	while(I2CMasterBusy(I2CA0_BASE)){}
	if(I2C_IF_Read(addr, rdata, 9)<0)			return -2;
	while(I2CMasterBusy(I2CA0_BASE)){}

	Xdata = (int)rdata[3];
	Xdata <<=8;
	Xdata += (int)rdata[4];

	Zdata = (int)rdata[5];
	Zdata <<=8;
	Zdata += (int)rdata[6];

	Ydata = (int)rdata[7];
	Ydata <<=8;
	Ydata += (int)rdata[8];


	mag_data->Hx = Xdata;
	mag_data->Hy = Ydata;
	mag_data->Hz = Zdata;

	return 0;
}



