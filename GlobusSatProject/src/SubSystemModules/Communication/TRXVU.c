/*
 * TRXVU.c
/*
 * trxuv_operations.c
 *
 *  Created on: Jul 4, 2012
 *      Author: marcoalfer
 */


#include <satellite-subsystems/IsisAntS.h>
#include <satellite-subsystems/IsisTRXVU.h>


#include "TRXVU.h"
#include "utils.h"

int InitTrxvu(){
	ISIStrxvuI2CAddress myTRXVUAddress;
	ISIStrxvuFrameLengths myTRXVUFramesLength ;

	//Buffer def
	myTRXVUFramesLength.maxAX25frameLengthTX=SIZE_TXFRAME;
	myTRXVUFramesLength.maxAX25frameLengthRX=SIZE_RXFRAME;

	//I2C add def
	myTRXVUAddress.addressVu_rc=I2C_TRXVU_RC_ADDR;
	myTRXVUAddress.addressVu_tc=I2C_TRXVU_TC_ADDR;

	//Bitrate def
	ISIStrxvuBitrate myTRXVUBitrates;
	myTRXVUBitrates=trxvu_bitrate_9600;
	if(logError(IsisTrxvu_initialize(&myTRXVUAddress,&myTRXVUFramesLength,&myTRXVUBitrates,1),"Initialize-TRXVU"))
		return -1;
	vTaskDelay(1000); //wait a little

    ISISantsI2Caddress myAntennaAddress;
    myAntennaAddress.addressSideA=ANTS_I2C_SIDE_A_ADDR;
    myAntennaAddress.addressSideB=ANTS_I2C_SIDE_B_ADDR;

    //init the ants sysytem
    if(logError(IsisTrxvu_initialize(&myAntennaAddress,&myTRXVUFramesLength,&myTRXVUBitrates,1),"InitTrxvu-IsisAntS_initialize"))
    	return -1;
//
//  InitTxModule();
//    InitBeaconParams();
//    InitSemaphores();
    //checkTransponderStart();//let us see if we need to turn on the tx

return 0;
}


