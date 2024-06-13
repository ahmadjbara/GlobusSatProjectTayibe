/*
 * EPS.c
 *
 *  Created on: 25 במאי 2024
 *      Author: 2022
 */
#include "EPS.h"
#include <satellite-subsystems/imepsv2_piu_types.h>
#include <hal\Drivers\SPI.h>
#include <FRAM_FlightParameters.h>


#define GetFilterVoltage(curr_voltage) (voltage_t)(alpha * curr_voltage + (1-alpha) * prev_avg)

int GetBatteryVoltage(voltage_t* c);
int EPS_Conditioning();
int UpdateAlpha(sat_packet_t *);

double prev_avg = 0;
float alpha = 0;
int EPS_Init(){

	IMEPSV2_PIU_t i2c_address;
	i2c_address.i2cAddr = EPS_I2C_ADDR;
	if(logError(IMEPSV2_PIU_Init(&i2c_address , 1) , "EPS_Init-ISIS")) return -1;

    if(logError(IsisSolarPanelv2_initialize(slave0_spi) , "EPS_Init- ISIS-solarPanel_init"))
			return 0;

   // if(GetThresholdVoltages(&eps_threshold_voltages)) return -1;

    if(GetAlpha(&alpha)){
    	alpha = DEFAULT_ALPHA_VALUE;
    }

    prev_avg = 0;
    GetBatteryVoltage(&prev_avg);

    return 0;
}


int EPS_Conditioning(){
	        voltage_t response1;
	        GetBatteryVoltage(&response1);
			voltage_t Current_volt = GetFilterVoltage(response1);

//			if(prev_avg == -1){
//				prev_avg = Current_volt;
//				return 0;
//			}
			// todo mv or volt
			if(Current_volt - prev_avg > 0) {
				if(Current_volt >= 7.5)
					printf("\t Full");
				else if(Current_volt < 7.5 && Current_volt >= 7.3 )
					printf("\t Normal");
				else
					printf("Safe");

			}else{
				if(Current_volt > 7.2 && Current_volt <= 7.4)
					printf("Normal");
				else if(Current_volt > 6.5 && Current_volt <= 7.2)
					printf("Safe");
				else
					printf("Critical");
			}

			prev_avg = Current_volt;
			return 0;
}
 // todo alpha must be a double value
int UpdateAlpha(sat_packet_t *cmd){

	float new_alpha = *((float*)cmd->data);
	if(new_alpha < 0 || new_alpha >1)
		return logError(-2, "UpdateAlpha");
	int err= logError(FRAM_write((unsigned char*)&new_alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE));
	if(err== E_NO_SS_ERR)
		GetAlpha(&alpha);
	return err;
}

int GetBatteryVoltage(voltage_t * c){
	imepsv2_piu__gethousekeepingeng__from_t hk_tlm;

	//if(logError(imepsv2_piu__gethousekeepingeng__from_t(EPS_I2C_BUS_INDEX , &hk_tlm) , "EPS "))
	return 0;
}



int GetAlpha(float *alpha){
	if(NULL == alpha)
		return E_INPUT_POINTER_NULL;
	if(logError(FRAM_read((unsigned char*)&alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE)))
		return -1;
	return 0;
}



