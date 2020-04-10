#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"
/***HAL STYLE NOT NEEDED, BUT RECOMMENDED***/

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.

    bool finished = false; //charFSM


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

    // TODO: Initialize EUSCI_A0

    /***see above function***/

    // TODO: Enable EUSCI_A0

    /***See above function***/

/*-----------------------------------------------------------------------*/
    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.


        if( UARTGetChar(EUSCI_A0_BASE) != -1 )
            rChar = UARTGetChar(EUSCI_A0_BASE);

        else
        {
        }

        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.

//so if I type 'h', echo h in MobaXterm and pass into charFSM as rChar. Check character first, then echo + update

           if( rChar != 0xFF )
           {
               UARTPutChar(EUSCI_A0_BASE, rChar);
               finished = charFSM(rChar);
           }

           else
           {
           }

           /*if( rChar != 0xFF )
           {
               //transmittedChar = rChar;
               UARTPutChar(EUSCI_A0_BASE, rChar); //questionable placement //is the char you received, if any received at all.
               charFSM(rChar);
           }*/

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.

           if( finished )
           {
               UARTPutChar(EUSCI_A0_BASE, &response);
           }

    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
//2534 is the sequence
bool charFSM(char rChar)
{
    static State currentState = START;
    bool finished = false;

    switch( currentState )
    {
        case START:
           if( rChar == 2 )
           {
               currentState = TWO;
           }
           break;

        case TWO:
            if( rChar == 5)
            {
                currentState = TWOFIVE;
            }

            else
            {
                currentState = START;
            }
            break;

        case TWOFIVE:
            if( rChar == 3 )
            {
                currentState = TWOFIVETHREE;
            }

            else
            {
                currentState = START;
            }
            break;

        case TWOFIVETHREE:
            if( rChar == 4 )
            {
                currentState = END;
                finished = true;
            }

            else
            {
                currentState = START;
            }
            break;
    }

    return finished;
}

void UARTSetup(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 *config)
{
    UART_initModule(moduleInstance, config);
    UART_enableModule(moduleInstance);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); //CAN I NOT USE #DEFINES FOR THE ARGUMENTS?
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
}

bool UARTHasChar(uint32_t moduleInstance)
{//nonblocking
    char dataReady = UART_getInterruptStatus(moduleInstance, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    if( dataReady ) //yes, data is here that needs to be received
        return true;

    else //no data to be received
        return false;
}

uint8_t UARTGetChar(uint32_t moduleInstance)
{ //returns uint8_t that corresponds to received byte

    bool dataReady = UARTHasChar(moduleInstance);
    char received;

    if( dataReady ) //check for data that needs to be received
       received = UART_receiveData(moduleInstance);

    else
        received = -1; //0xFF??

    return received;
}

bool UARTCanSend(uint32_t moduleInstance) //blocking
{
    char transmitRdy = UART_getInterruptStatus(moduleInstance, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);

    if( transmitRdy ) //if true to transmit now
        return true;
    else
        return false;
}

void UARTPutChar(uint32_t moduleInstance, uint8_t transmittedChar) //sends my byte
{
    bool transmitRdy = UARTCanSend(moduleInstance);

    while( transmitRdy ) //only doing this once ready to transmit
    {
            UART_transmitData(moduleInstance, transmittedChar);
            //transmitRdy = false;
            //transmittedChar = 0xFF;
    }
}

