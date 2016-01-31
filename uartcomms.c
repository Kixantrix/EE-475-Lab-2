#include "uartcomms.h"
#include "mcc_generated_files/eusart1.h"
#include <stdint.h>

/*
 * Takes a string and sends its contents over the uart.
 */
void sendString(char * str)
{
    while((*str) != '\0') {
         EUSART1_Write((uint8_t)(*str));
        str++;
    }
}

/*
 * Takes an integer and sends it over the uart.
 */
void sendInt(int toSend)
{
    char sendBuffer[16] = "";
    sendString(itoa(toSend, sendBuffer));
}

/*
 * itoa implementation, found here: 
 * http://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
 */
char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}