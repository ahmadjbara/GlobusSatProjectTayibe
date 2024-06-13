/*
 * InitSystem.c
 *
 *  Created on: 25 במאי 2024
 *      Author: 2022
 */


#include "utils.h"
#include <SubSystemModules\PowerManagment\EPS.h>
#include <hal\Drivers\I2C.h>
#include <hal\Drivers\SPI.h>
#include <InitSystem.h>

#define BusSpeed_Hz 100000
#define Timeout 10


int StartFRAM(){
	return logError(FRAM_start() , "Start_FRAM");
}
int StartI2C(){
	return logError(I2C_start(BusSpeed_Hz,Timeout),"Start_I2C");
}

int StartSPI(){

     //todo: make sure that the first parameter is both_spi
	return logError(SPI_start(both_spi, slave0_spi),"Start_SPI");
}

int StartTime(){
	int error = 0;
	Time expexted_deploy_time = UNIX_DATE_JAN_D1_Y2000;
	error = Time_start(&expexted_deploy_time,0);
	if(0 != error)
		return logError(error,"Start-Time-Time-Start");
	return 0;
}
void WriteDefaultValuesToFRAM(){


	FRAM_write(DEPLOYMENT_TIME,DEPLOYMENT_TIME_ADDR,DEPLOYMENT_TIME_SIZE);

	FRAM_write(SECONDS_SINCE_DEPLOY,SECONDS_SINCE_DEPLOY_ADDR,SECONDS_SINCE_DEPLOY_SIZE);

	FRAM_write(DEFAULT_NO_COMM_WDT_KICK_TIME,NO_COMM_WDT_KICK_TIME_ADDR,NO_COMM_WDT_KICK_TIME_SIZE);

	FRAM_write(STOP_REDEPOLOY_FLAG,STOP_REDEPOLOY_FLAG_ADDR,STOP_REDEPOLOY_FLAG_SIZE);

	//??
	//FRAM_write(......,MUTE_END_TIME_ADDR,MUTE_END_TIME_SIZE);

	FRAM_write(FIRST_ACTIVATION_FLAG,FIRST_ACTIVATION_FLAG_ADDR,FIRST_ACTIVATION_FLAG_SIZE);



}
int Init_Drivers(){
	StartI2C();
	StartSPI();
	StartFRAM();
	StartTime();

}
int Init_SubSystems(){
	EPS_Init();
	InitializeFS();
	InitTrxvu();
}
int InitSystems(){
	Init_Drivers();
	Init_SubSystems();
}
