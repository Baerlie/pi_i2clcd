
/*
*
* by Lewis Loflin www.bristolwatch.com lewis@bvu.net
* http://www.bristolwatch.com/rpi/i2clcd.htm
* Using wiringPi by Gordon Henderson
*
*
* Port over lcd_i2c.py to C and added improvements.
* Supports 16x2 and 20x4 screens.
* This was to learn now the I2C lcd displays operate.
* There is no warrenty of any kind use at your own risk.
*
*/

/*
*
* adaptions by B. Pscheidl for 20x4 LCD
* and pigpio http://abyz.me.uk/rpi/pigpio/
*
*/

#include <pigpio.h> // -lpigpio -lrt
#include <stdlib.h>
#include <stdio.h>

// Define some device parameters
#define I2C_ADDR   0x27 // I2C device address

// Define some device constants
#define LCD_CHR  1 // Mode - Sending data
#define LCD_CMD  0 // Mode - Sending command

#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line
#define LINE3  0x94 // 3rd line
#define LINE4  0xD4 // 4th line

#define LCD_BACKLIGHT   0x08  // On
// LCD_BACKLIGHT = 0x00  # Off

#define ENABLE  0b00000100 // Enable bit

void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// added by Lewis
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);
int fd;  // seen by all subroutines

//Test program
int main_()   {
  if (gpioInitialise() < 0)
    exit (1);

  fd = i2cOpen(1, I2C_ADDR, 0);

  lcd_init(); // setup LCD

  char array1[] = "Hello world!";

  while (1)   {
    lcdLoc(LINE1);
    typeln("Using wiringPi");
    lcdLoc(LINE2);
    typeln("Geany editor.");

    gpioDelay(2000000);
    ClrLcd();
    lcdLoc(LINE1);
    typeln("I2c  Programmed");
    lcdLoc(LINE2);
    typeln("in C not Python.");

    gpioDelay(2000000);
    ClrLcd();
    lcdLoc(LINE1);
    typeln("Arduino like");
    lcdLoc(LINE2);
    typeln("fast and easy.");

    gpioDelay(2000000);
    ClrLcd();
    lcdLoc(LINE1);
    typeln(array1);

    //4 lines test
    gpioDelay(2000000);
    ClrLcd();
    lcdLoc(LINE1);
    typeln(array1);
    lcdLoc(LINE2);
    typeln("Line 2 on 20x4");
    lcdLoc(LINE3);
    typeln("Added a 3rd line!");
    lcdLoc(LINE4);
    typeln("And a 4th for fun...");


    gpioDelay(2000000);
    ClrLcd(); // defaults LINE1
    typeln("Int  ");
    int value = 20125;
    typeInt(value);

    gpioDelay(2000000);
    lcdLoc(LINE2);
    typeln("Float ");
    float FloatVal = 10045.25989;
    typeFloat(FloatVal);
    gpioDelay(4000000);
  }

  return 0;
}


// float to string
void typeFloat(float myFloat)   {
  char buffer[20];
  sprintf(buffer, "%4.2f",  myFloat);
  typeln(buffer);
}

// int to string
void typeInt(int i)   {
  char array1[20];
  sprintf(array1, "%d",  i);
  typeln(array1);
}

// clr lcd go home loc 0x80
void ClrLcd(void)   {
  lcd_byte(0x01, LCD_CMD);
  lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void lcdLoc(int line)   {
  lcd_byte(line, LCD_CMD);
}

// out char to LCD at current position
void typeChar(char val)   {
  lcd_byte(val, LCD_CHR);
}


// this allows use of any size string
void typeln(const char *s)   {
  while ( *s ) lcd_byte(*(s++), LCD_CHR);
}

void lcd_byte(int bits, int mode)   {

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT ;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT ;

  // High bits
  i2cReadByteData(fd, bits_high);
  lcd_toggle_enable(bits_high);

  // Low bits
  i2cReadByteData(fd, bits_low);
  lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits)   {
  // Toggle enable pin on LCD display
  gpioDelay(500);
  i2cReadByteData(fd, (bits | ENABLE));
  gpioDelay(500);
  i2cReadByteData(fd, (bits & ~ENABLE));
  gpioDelay(500);
}


void lcd_init()   {
  // Initialise display
  lcd_byte(0x33, LCD_CMD); // Initialise
  lcd_byte(0x32, LCD_CMD); // Initialise
  lcd_byte(0x06, LCD_CMD); // Cursor move direction
  lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
  lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
  lcd_byte(0x01, LCD_CMD); // Clear display
  gpioDelay(500);
}
