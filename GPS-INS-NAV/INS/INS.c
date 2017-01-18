/*
 * INS.c
 *
 *  Created on: 11/1/17
 *      Author: Gur Chemel
 */

#include "../includes/std_inc.h"
#include "../sensors/sensors.h"
#include "../main.h"
#include "INS.h"
#include "../includes/infrastructure.h"

// 3*3 Matrix multiplication. out_vec=matrix*in_vec.
void mat_dot_vec(double matrix[3][3], double in_vec[3], double out_vec[3])
{
	out_vec[0]=(matrix[0][0]) * (in_vec[0]) + (matrix[0][1]) * (in_vec[1]) + (matrix[0][2]) * (in_vec[2]);
	out_vec[1]=(matrix[1][0]) * (in_vec[0]) + (matrix[1][1]) * (in_vec[1]) + (matrix[1][2]) * (in_vec[2]);
	out_vec[2]=(matrix[2][0]) * (in_vec[0]) + (matrix[2][1]) * (in_vec[1]) + (matrix[2][2]) * (in_vec[2]);
}

// INS_init: Set the system initial state (in ENU), angle (in RPY) and reference point (in ECEF)
void INS_init(system_state_str* systemState, double localRef[3], double enuToEcefMat[3][3])
{

	// Set initial Position and velocity in ENU to 0.
	systemState->Px=0;
	systemState->Py=0;
	systemState->Pz=0;
	systemState->Vx=0;
	systemState->Vy=0;
	systemState->Vz=0;

	// Calculate Roll, Pitch, and Yaw according to the Magnetometer and Accelerometer.
	acc_input_data_str acc_data;			//	acc_data.Ax , acc_data.Ay , acc_data.Az
	get_acc_data(&acc_data);
	mag_input_data_str mag_data;			//	mag_data.Hx , mag_data.Hy , mag_data.Hz
	get_mag_data(&mag_data);

	// Algorithm used by Rafael Taub.
	double phi = atan2(acc_data.Ay, acc_data.Az);
	double sinphi = sin(phi);
	double cosphi = cos(phi);
	double theta = atan(-acc_data.Ax/(acc_data.Ay*sinphi + acc_data.Az*cosphi));
	double sintheta = sin(theta);
	double costheta = cos(theta);
	double psi = atan2(mag_data.Hz*sinphi - mag_data.Hy*cosphi, mag_data.Hx*costheta + mag_data.Hy*sintheta*sinphi + mag_data.Hz*sintheta*cosphi);

	systemState->Roll=phi*rad2deg;
	systemState->Pitch=theta*rad2deg;
	systemState->Yaw=psi*rad2deg;

	// Average GPS_AVERAGE_LENGTH GPS data readings and save as local reference from ECEF zero to ENU zero (In ECEF coordinate).
	gps_input_data_str gps_data;			//	gps_data.X	, gps_data.Y  , gps_data.Z
	gps_input_data_str gps_data_mean;	//	gps_data.X	, gps_data.Y  , gps_data.Z
	gps_data_mean.X=0;
	gps_data_mean.Y=0;
	gps_data_mean.Z=0;
	int i=0;
	for (i=0;i<GPS_AVERAGE_LENGTH;i++){
		get_gps_data(&gps_data);						// TODO: Set waiting period of at least 1 sec.
		gps_data_mean.X=gps_data_mean.X+gps_data.X;
		gps_data_mean.Y=gps_data_mean.Y+gps_data.Y;
		gps_data_mean.Z=gps_data_mean.Z+gps_data.Z;
	}
	localRef[X_pos]=gps_data_mean.X/GPS_AVERAGE_LENGTH;
	localRef[Y_pos]=gps_data_mean.Y/GPS_AVERAGE_LENGTH;
	localRef[Z_pos]=gps_data_mean.Z/GPS_AVERAGE_LENGTH;


	// Calculate enuToEcefMat according to theta and phi from gps.
	double phi_eccef = atan2(gps_data_mean.Y, gps_data_mean.Z);
	double sinph = sin(phi_eccef);
	double cosph = cos(phi_eccef);

	double theta_eccef = atan2(-gps_data_mean.X, gps_data_mean.Y*sinph + gps_data_mean.Z*cosph);
	double sinth = sin(theta_eccef);
	double costh = cos(theta_eccef);

	enuToEcefMat[0][0]=(-sinth);
	enuToEcefMat[0][1]=(-costh*sinph);
	enuToEcefMat[0][2]=(costh*cosph);
	enuToEcefMat[1][0]=(costh);
	enuToEcefMat[1][1]=(-sinth*sinph);
	enuToEcefMat[1][2]=(sinth*cosph);
	enuToEcefMat[2][0]=(0);
	enuToEcefMat[2][1]=(cosph);
	enuToEcefMat[2][2]=(sinph);
}


void INS_calc(system_state_str* systemState)
{
	// Build RPYtoENU matrix according to Roll,Pitch,Yaw.
	double sr = sin(deg2rad*systemState->Roll);
	double cr = cos(deg2rad*systemState->Roll);
	double sp = sin(deg2rad*systemState->Pitch);
	double cp = cos(deg2rad*systemState->Pitch);
	double sy = sin(deg2rad*systemState->Yaw);
	double cy = cos(deg2rad*systemState->Yaw);

	double rpy_to_enu_mat[3][3];
	rpy_to_enu_mat[0][0]=sy*cp;
	rpy_to_enu_mat[0][1]=( cr*cy+sr*sp*sy);
	rpy_to_enu_mat[0][2]=(-sr*cy+cr*sp*sy);
	rpy_to_enu_mat[1][0]=cy*cp;
	rpy_to_enu_mat[1][1]=(-cr*sy+sr*sp*cy);
	rpy_to_enu_mat[1][2]=( sr*sy+cr*sp*cy);
	rpy_to_enu_mat[2][0]=sp;
	rpy_to_enu_mat[2][1]=(-sr*cp);
	rpy_to_enu_mat[2][2]=(-cr*cp);

	// Get acceleration in RPY and convert them to ENU.
	acc_input_data_str acc_data;			//	acc_data.Ax , acc_data.Ay , acc_data.Az
	get_acc_data(&acc_data);
	double acc_dt = acc_data.time; // TODO: Gur NOTICE: i changed this to read time straight fron acc_data struct
	double rpy_accel[3];
	rpy_accel[X_pos]=acc_data.Ax;
	rpy_accel[Y_pos]=acc_data.Ay;
	rpy_accel[Z_pos]=acc_data.Az;
	double enu_accel[3];
	mat_dot_vec(rpy_to_enu_mat,rpy_accel,enu_accel);
	enu_accel[Z_pos]=enu_accel[Z_pos]+G_VALUE;	// Compensate on earth's Gravitational force on the Up axis.

	// Calculate new Position and velocity.
	systemState->Px=(systemState->Px)+(systemState->Vx)*acc_dt+0.5*(rpy_accel[X_pos])*(acc_dt*acc_dt);
	systemState->Py=(systemState->Py)+(systemState->Vy)*acc_dt+0.5*(rpy_accel[Y_pos])*(acc_dt*acc_dt);
	systemState->Pz=(systemState->Pz)+(systemState->Vz)*acc_dt+0.5*(rpy_accel[Z_pos])*(acc_dt*acc_dt);
	systemState->Vx=(systemState->Vx)+(rpy_accel[X_pos])*acc_dt;
	systemState->Vy=(systemState->Vy)+(rpy_accel[Y_pos])*acc_dt;
	systemState->Vz=(systemState->Vz)+(rpy_accel[Z_pos])*acc_dt;

	// Get velocity in Roll Pitch Yaw and calculate new values.
	gyr_input_data_str gyr_data;			//	gyr_data.Wr , gyr_data.Wp , gyr_data.Wy
	get_gyr_data(&gyr_data);
	double gyr_dt= gyr_data.time;		//TODO: same here

	systemState->Roll=(systemState->Roll)+gyr_dt*(gyr_data.Wr);
	systemState->Pitch=(systemState->Pitch)+gyr_dt*(gyr_data.Wp);
	systemState->Yaw=(systemState->Yaw)+gyr_dt*(gyr_data.Wy);

}