/*
 * EPS.c
 *
 *  Created on: 25 ���� 2024
 *      Author: 2022
 */
#include "EPS.h"
#include <satellite-subsystems/imepsv2_piu_types.h>
#include <satellite-subsystems/imepsv2_piu.h>
#include <hal\Drivers\SPI.h>
#include <FRAM_FlightParameters.h>
#include <utils.h>
#include <satellite-subsystems/GomEPS.h>

#define GetFilterVoltage(curr_voltage) (voltage_t)(alpha * curr_voltage + (1-alpha) * prev_avg)



int GetBatteryVoltage(voltage_t* c);
int EPS_Conditioning();
int UpdateAlpha(sat_packet_t *);


double prev_avg = 0;
float alpha = 0;
#ifdef Gom
Boolean EPS_Init(void)
{
    unsigned char i2c_address = 0x02;
    int rv;

	rv = GomEpsInitialize(&i2c_address, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)
	{
		if(logError(rv,"GomEpsInitialize() failed"))
			return -1;

	}

	if(GetAlpha(&alpha)){
	    	alpha = DEFAULT_ALPHA_VALUE;
	    }

	    prev_avg = 0;
	    GetBatteryVoltage(&prev_avg);

	return 0;
}
#else
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
#endif

int EPS_Conditioning(){
	        voltage_t response1;
#ifdef GoM
	        Gom_GetBatteryVoltage(&response1);
#else
	        GetBatteryVoltage(&response1);
#endif
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
	int err= logError(FRAM_write((unsigned char*)&new_alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE),"write to fram");
	if(err== E_NO_SS_ERR)
		GetAlpha(&alpha);
	return err;
}

int GetBatteryVoltage(voltage_t * c){

	imepsv2_piu__gethousekeepingeng__from_t response;

		int error = imepsv2_piu__gethousekeepingeng(0,&response);
		if( error )
		{
			if(logError(error,"imepsv2_piu__gethousekeepingeng(...)"))
				return -1;
			return 0;
		}

		*c=response.fields.batt_input.fields.volt;
		return 0;
}

int Gom_GetBatteryVoltage(voltage_t * c){


	gom_eps_hk_t response;

		int error = imepsv2_piu__gethousekeepingeng(0,&response);
		if( error )
		{
			if(logError(error,"imepsv2_piu__gethousekeepingeng(...) return error (%d)!\n\r"))
				return -1;
		}

		*c=response.fields.vbatt;
		return 0;
}



int GetAlpha(float *alpha){
	if(NULL == alpha)
		return E_INPUT_POINTER_NULL;
	if(logError(FRAM_read((unsigned char*)&alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE),"reading alfa to fram"))
		return -1;
	return 0;
}



