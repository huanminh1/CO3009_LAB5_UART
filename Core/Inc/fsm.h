/*
 * fsm.h
 *
 *  Created on: Nov 12, 2025
 *      Author: admin
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"
#include <stdio.h>

#define MAX_BUFFER_SIZE 30
#define FALSE 0
#define TRUE 1

typedef enum {
    CMD_START,
	CMD_GET,
} CommandState;

typedef enum {
	INIT,
    STOP_PRINT_VALUE,
	PRINT_VALUE,
} Print_State;


void clock();
void readADC();
void uart_communiation_fsm();
void command_parser_fsm();
void get_byte(uint8_t temp);
void reset_flag();
uint8_t is_buffer_flag();

extern uint8_t buffer_flag;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
#endif /* INC_FSM_H_ */
