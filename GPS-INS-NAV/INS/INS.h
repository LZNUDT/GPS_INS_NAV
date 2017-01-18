/*
 * INS.h
 *
 *  Created on: 12 ����� 2017
 *      Author: ���
 */

#ifndef INS_H_
#define INS_H_

void mat_dot_vec(double [][], double [], double []);
void INS_init(system_state_str* , double [], double [][]);
void INS_calc(system_state_str* );

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#ifndef G_VALUE
#define G_VALUE 9.81
#endif

#define deg2rad M_PI/180
#define rad2deg 180/M_PI
#define GPS_AVERAGE_LENGTH 5

#endif /* INS_H_ */
