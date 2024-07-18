/*
 * utils.c
 *
 *  Created on: Jun 29, 2024
 *      Author: tariq
 */
#include "utils.h"

int logError(int error ,char* msg){
	if(error != E_NO_SS_ERR)
		printf("got %d  from , %s",error,msg);
	return 0;
}
