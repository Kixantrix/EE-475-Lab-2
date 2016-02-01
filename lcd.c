#include <stdint.h>

#include "xc.h"
#include "mcc_generated_files/mcc.h"
#define I2C_IO_V1 //make sure we use the right pins
#include "i2c.h"
//#include "utils.h"
#include "mcc_generated_files/i2c1.h"


/*void I2C1_Initialize(void) {
    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP Sample At Middle; UA dontupdate; CKE Idle to Active; D_nA lastbyte_address; 
    SSP1STAT = 0x00;
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low, Active:High; SSPM FOSC/4_SSPxADD; SSPOV no_overflow; 
    SSP1CON1 = 0x28;
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    // Baud Rate Generator Value: SSP1ADD 3;   
    SSP1ADD = 0x03;
}*/

void LCD_Open()
{
    IdleI2C();
    OpenI2C(MASTER, SLEW_OFF);           // Begin transfer, claim I2C BUS
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(LCD_PIC_I2C_ADDR);          // Tell all I2C devices you are talking to LCD_PIC_I2C_ADDR
}
void LCD_Close()
{
    IdleI2C();
    CloseI2C();
}

void LCD_BL(uint8_t status)
{
    LCD_BL_Status = status;
    LCD_Write_Byte(0x00, 0x00);
}

void LCD_Init()
{
    I2C1_Initialize();
    LCD_Open();
    
    // Following bytes are all Command bytes, i.e. address = 0x00
    LCD_Write_Byte(0x00, 0x03);   // Write Nibble 0x03 three times (per HD44780U initialization spec)
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x03);   //
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x03);   //
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x02);   // Write Nibble 0x02 once (per HD44780U initialization spec)
    __delay_ms(5);
    LCD_Write_Byte(0x00, 0x02);   // Write Nibble 0x02 once (per HD44780U initialization spec)
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x01);   // Set mode: 4-bit, 2+lines, 5x8 dots
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x0C);   // Display ON 0x0C
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x01);   // Clear display
    __delay_ms(5);                // (per HD44780U initialization spec)
    LCD_Write_Byte(0x00, 0x06);   // Set cursor to increment
    __delay_ms(5);                // (per HD44780U initialization spec)
    
    LCD_Close();
}

void LCD_Goto(uint8_t x, uint8_t y)
{
uint8_t address;

   switch(y)
     {
      case 1:
        address = LCD_PIC_LINE_1_ADDRESS;
        break;

      case 2:
        address = LCD_PIC_LINE_2_ADDRESS;
        break;

      case 3:
        address = LCD_PIC_LINE_3_ADDRESS;
        break;

      case 4:
        address = LCD_PIC_LINE_4_ADDRESS;
        break;

      default:
        address = LCD_PIC_LINE_1_ADDRESS;
        break;
     }

   address += x-1;
   LCD_Write_Byte(0, 0x80 | address);
}

//===================================
void LCD_Write_String(const char *str)
{
   // Writes a string text[] to LCD via I2C
   pin_RS  = 1;
   pin_RW  = 0;
   pin_E   = 0;
   pin_BL  = LCD_BL_Status;

   while (*str)
   {
        // Send upper nibble
        _LCD_Write_Upper_Nibble(*str);

        // Send lower nibble
        _LCD_Write_Lower_Nibble(*str);

        str++;
   }
}

void LCD_Write_Char(char c)
{
    LCD_Write_Byte(0x01,c);
}

void LCD_Write_Int(int32_t num)
{
    if (num < 0) { LCD_Write_String("-"); num *= -1; }

    uint8_t number[10];
    uint8_t num_count = 0;

    do {        
        number[num_count] = num % 10;
        num_count++;
        num /= 10;
    } while (num > 0);

    for (int i = num_count-1; i>= 0; i--)
    {
        LCD_Write_Char(number[i] + 0b00110000);
    }
}

//===================================
void LCD_Write_Byte(uint8_t address, uint8_t n)
{
    if (address)
    {
        pin_RS=1;   // Data
    }
    else
    {
        pin_RS=0;   // Command
    }

    pin_RW  = 0;
    pin_E   = 0;
    pin_BL  = LCD_BL_Status;

    // Send upper nibble
   _LCD_Write_Upper_Nibble(n);

    // Send lower nibble
   _LCD_Write_Lower_Nibble(n);
}

void LCD_Clear()
{
    LCD_Write_Byte(0x00,0x01);
    __delay_ms(5);
}

void LCD_Clear_Line(uint8_t line)
{
    LCD_Goto(1,line);
    for (int i = 0; i<20; i++)
    {
        LCD_Write_String(" ");
    }
    LCD_Goto(1,line);
}

void _LCD_Write_Upper_Nibble(uint8_t u)
{
    // Send upper nibble
    if(bit_test(u,7))
        pin_D7=1;
    else
        pin_D7=0;

    if(bit_test(u,6))
        pin_D6=1;
    else
        pin_D6=0;

    if(bit_test(u,5))
        pin_D5=1;
    else
        pin_D5=0;

    if(bit_test(u,4))
        pin_D4=1;
    else
        pin_D4=0;

   pin_E = 0;
   WriteI2C(_LCD_Build_Byte());
   pin_E = 1;
   WriteI2C(_LCD_Build_Byte());
   pin_E = 0;
   WriteI2C(_LCD_Build_Byte());
}

void _LCD_Write_Lower_Nibble(uint8_t l)
{
    // Send lower nibble
    if(bit_test(l,3))
        pin_D7=1;
    else
        pin_D7=0;

    if(bit_test(l,2))
        pin_D6=1;
    else
        pin_D6=0;

    if(bit_test(l,1))
        pin_D5=1;
    else
        pin_D5=0;

    if(bit_test(l,0))
        pin_D4=1;
    else
        pin_D4=0;

    pin_E = 0;
    WriteI2C(_LCD_Build_Byte());
    pin_E = 1;
    WriteI2C(_LCD_Build_Byte());
    pin_E = 0;
    WriteI2C(_LCD_Build_Byte());
}

uint8_t _LCD_Build_Byte()
{
    uint8_t ret = 0x00;

    ret |= pin_E    << 2;
    ret |= pin_RW   << 1;
    ret |= pin_RS   << 0;
    ret |= pin_D4   << 4;
    ret |= pin_D5   << 5;
    ret |= pin_D6   << 6;
    ret |= pin_D7   << 7;
    ret |= pin_BL   << 3;

    return ret;
}
