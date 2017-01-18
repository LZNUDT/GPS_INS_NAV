/*
 * HMC5883.h
 *
 *  Created on: Jun 6, 2016
 *      Author: Rafi
 */

#ifndef HMC5883_H_
#define HMC5883_H_

typedef int Mag_field;
typedef struct {
	Mag_field Hx;
	Mag_field Hy;
	Mag_field Hz;
}Mag_inner_data_str;

int init_HMC5883(char addr, char cal);
int HMC5883_read_magdata(char addr, Mag_inner_data_str* mag_data);
void convert_mag_raw_data_2_units(Mag_inner_data_str* mag_input);


#endif /* HMC5883_H_ */