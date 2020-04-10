/*
 * homework4.h
 *
 *  Created on:
 *      Author:
 */

#ifndef HOMEWORK4_H_
#define HOMEWORK4_H_

// This function initializes the board by turning off the Watchdog Timer.
void initBoard();
bool charFSM(char rChar);

// TODO: Define any constants that are local to homework.c using #define
//#define GPIO_PIN2 PIN2
//#define GPIO_PIN3 PIN3
//#define GPIO_PORT_P1 P1
//#define GPIO_PRIMARY_MODULE_FUNCTION PRIMARY
typedef enum state {START, TWO, TWOFIVE, TWOFIVETHREE, END} State; //setting desired states

#endif /* HOMEWORK4_H_ */
