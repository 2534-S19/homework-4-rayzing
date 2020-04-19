#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"
/***HAL STYLE NOT NEEDED, BUT RECOMMENDED***/

int main(void)

{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.

    bool finished = false; //charFSM
    int incr = 0; //incrementing for transmitting response string
    bool transmitRdy = UARTCanSend(EUSCI_A0_BASE); //detects whether UART transmit is ready
    /*int length = 0;*/


    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baurd, 8-bit payload (LSB first), no parity, 1 stop bit.
    //check UART.h for config defines
    eUSCI_UART_ConfigV1 uartSetup =
    {
         EUSCI_A_UART_CLOCKSOURCE_SMCLK, //System Clock Board = 3MHz
         19,
         8,
         0x55,
         EUSCI_A_UART_NO_PARITY,
         EUSCI_A_UART_LSB_FIRST,
         EUSCI_A_UART_ONE_STOP_BIT,
         EUSCI_A_UART_MODE,
         EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
         EUSCI_A_UART_8_BIT_LEN
    };

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.

    UARTSetup(EUSCI_A0_BASE, &uartSetup);
    /*length = myStringLength(response);*/

    // TODO: Initialize EUSCI_A0

    /***see above function***/

    // TODO: Enable EUSCI_A0

    /***See above function***/

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.

        rChar = UARTGetChar(EUSCI_A0_BASE); //store data in rChar for use later


        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.

//so if I type 'h', echo h in MobaXterm and pass into charFSM as rChar. Check character first, then echo + update



        if( rChar != 0xFF ) //actual char received, check transmit flag; if rdy, echo it & update FSM
        {
            UARTPutChar(EUSCI_A0_BASE, rChar); //transmitting rChar if not 0xFF as long as transmit is rdy
            finished = charFSM(rChar); //updating for response string rdy
        }

        else //no char received, so don't do anything
        {
        }

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission. --THATS YOUR *FINISHED*

        //if( finished ) //extra test measure
        //{
           while( finished && response[incr] != '\0' ) //once char set is complete and fully reading 2534
           {
               if( transmitRdy )
               {
                   UART_transmitData(EUSCI_A0_BASE, response[incr]); //transmit string one char at a time
                   incr++; //must be below length
               }

           }

        incr = 0; //reset incr variable for next 2534 reading
        finished = false; //reset success var after transmitting
        //}




    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
//2534 is the sequence
/********************************************************************************
Definition of function charFSM: Detect 2534 sequence and output success if found
Outputs: finished boolean variable
*********************************************************************************/
bool charFSM(char rChar)
{
    static State currentState = START; //start here when entering
    bool finished = false;

    switch( currentState ) //detects current state and whether or not to admit string
    {
        case START:
           if( rChar == '2' )
           {
               currentState = TWO;
           }
           break;

        case TWO:
            if( rChar == '5' )
            {
                currentState = TWOFIVE;
            }

            else if( rChar == '2' )
            {
                currentState = TWO;
            }

            else
            {
                currentState = START;
            }
            break;

        case TWOFIVE:
            if( rChar == '3' )
            {
                currentState = TWOFIVETHREE;
            }

            else
            {
                currentState = START;
            }
            break;

        case TWOFIVETHREE:
            if( rChar == '4' )
            {
                currentState = END;
                finished = true;
            }

            else
            {
                currentState = START;
            }
            break;

        case END:
            if( rChar == '2')
            {
                currentState = TWO;
                finished = false; //reset
            }

            else
            {
                currentState = START;
                finished = false; //reset
            }
            break;
    }

    return finished;
}

/*unsigned int myStringLength(char *str) //so this points to char str1 first address as a place to start reading.
{
    unsigned length = 0; //counter
    int incr = 0;

        while( str[incr] != '\0' ) //while the contents I'm pointing to aren't 0
        {
            length++; //there's a character there so increment the length
            str++; //I want to move to the consecutive next address so that I can read its contents, too.
        }

    return length;
}*/
/*************************************************************************************
Definition of function UARTSetup: To initialize everything and enable the UART module
Outputs: None
*************************************************************************************/
void UARTSetup(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 *config)
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(moduleInstance, config);
    UART_enableModule(moduleInstance);
}

/*********************************************
Definition of function UARTHasChar:
To see if UART is available to receive a char
Outputs: boolean T/F
*********************************************/
bool UARTHasChar(uint32_t moduleInstance)
{//nonblocking
    int dataReady = UART_getInterruptStatus(moduleInstance, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    if( dataReady = EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG ) //yes, data is here that needs to be received
        return true;
    else //no data to be received
        return false;
}

/***********************************
Definition of function UARTGetChar:
To get the character to be received
Outputs: An unsigned int (8char)
***********************************/
uint8_t UARTGetChar(uint32_t moduleInstance)
{ //returns uint8_t that corresponds to received byte

    bool dataReady = UARTHasChar(moduleInstance); //formal
    uint8_t received; //formal

    if( dataReady ) //Check if rdy to receive data
       received = UART_receiveData(moduleInstance); //store whatever was received in this var
    else
       received = 0xFF;

    return received;
}

/*************************************
Definition of function UARTCanSend
To check if UART is ready to transmit
Outputs: Boolean T/F
*************************************/
bool UARTCanSend(uint32_t moduleInstance) //blocking
{
    int transmitRdy = UART_getInterruptStatus(moduleInstance, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);

    if( transmitRdy == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG ) //if true to transmit now
        return true;
    else
        return false;
}

/**********************************************
Definition of function UARTPutChar
To transmit when rdy. This only works for type
not response string
Outputs: N/A directly but transmits
**********************************************/
void UARTPutChar(uint32_t moduleInstance, uint8_t transmittedChar) //sends my byte
{
    bool transmitRdy = UARTCanSend(moduleInstance);

    while( transmitRdy ) //only doing this once ready to transmit STOPPED HERE
    {
            UART_transmitData(moduleInstance, transmittedChar);
            transmitRdy = false;
    }
}

