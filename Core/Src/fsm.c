/*
 * fsm.c
 *
 *  Created on: Nov 12, 2025
 *      Author: admin
 */
#include "fsm.h"

// ----------------Command buffer------------------------
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t in_byte = 0;
uint8_t buffer_flag = 0;

// ----------------Clock------------------------
const int MAX_COUNT = 300;
int counter = 300;
uint8_t enableClock = FALSE;

// ---------------Print ADC Value-------------------------
uint8_t isRST = FALSE;
uint8_t isOK = FALSE;
uint8_t isTIMEOUT = FALSE;
uint32_t ADC_value = 0;
char str[32];

// ----------------------------------------
CommandState cmdState = CMD_START;
Print_State printState = INIT;

// ----------------Get RX_Byte-----------------------
void get_byte(uint8_t temp)
{
	in_byte = temp;
	if (temp == 8){		//handle backspace
		if(index_buffer > 0) index_buffer--;
	}
	else{
		buffer[index_buffer++] = temp;
		buffer_flag = 1;
	}

	if (index_buffer == MAX_BUFFER_SIZE) index_buffer = 0;
}


// ----------------Command_parser_FSM------------------------
void command_parser_fsm(){
	switch(cmdState){
		case CMD_START:
			if(in_byte == '!'){
				index_buffer = 0;
				cmdState = CMD_GET;
			}
			break;
		case CMD_GET:
			if(in_byte == '!'){
				index_buffer = 0;
			}
			if(in_byte == '#'){
				/*----------------------RST Command----------------------*/
				if (buffer[0] == 'R' && buffer[1] == 'S'  && buffer[2] == 'T' && buffer[3] == '#'){
					isRST = TRUE;
				}
				/*----------------------OK Command----------------------*/
				else if (buffer[0] == 'O' && buffer[1] == 'K'  && buffer[2] == '#'){
					isOK = TRUE;
				}
				cmdState = CMD_START;
			}
			break;
		default:
			break;
	}
}
void uart_communiation_fsm(){
	switch(printState){
		case INIT:
			if(1){
				printState = STOP_PRINT_VALUE;
			}
			break;
		case STOP_PRINT_VALUE:
			if(isRST == TRUE){
				readADC();
				enableClock = TRUE;
				counter = MAX_COUNT;
				isRST = FALSE;
				printState = PRINT_VALUE;
			}
			break;
		case PRINT_VALUE:
			if(isTIMEOUT == TRUE){
				HAL_UART_Transmit(&huart2, (void *)str, sprintf(str, "!ADC=%ld#\r\n", ADC_value), 1000);
				isTIMEOUT = FALSE;
			}
			if(isOK == TRUE){
				enableClock = FALSE;
				counter = MAX_COUNT;
				isOK = FALSE;
				printState= STOP_PRINT_VALUE;
			}
			if(isRST == TRUE){
				readADC();
				enableClock = TRUE;
				counter = MAX_COUNT;
				isRST = FALSE;
			}
			break;
		default:
			break;
	}
}

// --------------Time counting--------------------------

void clock(){
	if(enableClock == TRUE){
		if (counter == 0){
			isTIMEOUT = TRUE;
			counter = MAX_COUNT;
		} else {
			counter--;
		}
	}
}

// --------------------ADC Reading--------------------

void readADC(){
	ADC_value = HAL_ADC_GetValue(&hadc1);		//Read ADC
	HAL_UART_Transmit(&huart2, (void *)str, sprintf(str, "!ADC=%ld#\r\n", ADC_value), 1000);
}

// --------------------Reset buffer flag--------------------
void reset_flag(){
	buffer_flag = 0;
}

uint8_t is_buffer_flag(){
	if(buffer_flag == 1) return 1;
	return 0;
}
