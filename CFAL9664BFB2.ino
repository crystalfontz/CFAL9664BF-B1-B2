//
//  Sketch written for Seeeduino v4.2 set to 3.3v (important!)
//  Seeduino is an Arduino Uno clone, with built-in 3.3v support.
//
//  CRYSTALFONTZ CFAL9664B-F-B1 or CFAL9664B-F-B2 96x64 COLOR 0.95" OLED
//
//  This code uses the 4-wire SPI mode of the display. Actually only 3 wires
//  since MISO is unused.
//
//   https://www.crystalfontz.com/product/cfal9664bfb1
//   https://www.crystalfontz.com/product/cfal9664bfb2
//
//  video: https://www.youtube.com/watch?v=
//
//  2018 - 07 - 02 Brent A. Crosby
//  2018 - 10 - 08 Trevin Jorgenson
//===========================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//============================================================================
//
// Display is Crystalfontz CFAL9664B-F-B1 or CFAL9664B-F-B2
//   https://www.crystalfontz.com/product/cfal9664bfb1
//   https://www.crystalfontz.com/product/cfal9664bfb2
//
// The controller is a SSD1331
//   https://www.crystalfontz.com/controllers/SolomonSystech/SSD1331/
//


// Set Display Type:
#define CFAL9664BFB1 (0) 
#define CFAL9664BFB2 (1)


// The two varaints have different current requirements.
//============================================================================
//Set BUILD_SD to enable a slide show from the uSD card to the OLED.
//Files need to be Windows 96x64 24-bit BMP, file size will be
//exactly 18,488 bytes.
#define BUILD_SD (0)
//============================================================================
#include <avr/io.h>
#include <SPI.h>
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SPI\src\SPI.cpp
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SPI\src\SPI.h
#include <util/delay.h>
#include <avr/pgmspace.h>

#if BUILD_SD
#include <SD.h>
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.cpp
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.h

/* You might want to make these changes to the SD.cpp library
   function SDClass::begin() to allow multiple calls

//ref: https://github.com/arduino-libraries/SD/issues/5
// ----> Line Added to beginning of SDClass::begin
  if (root.isOpen()) root.close();      // allows repeated calls
// <---- Line Added

And this change to make speed reasonable:

  //  return card.init(SPI_HALF_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // to:
  //
  //  return card.init(SPI_FULL_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);

*/
#endif

//============================================================================
// CFAL9664B-F-Bx on CFA-10083 Carrier Board
// OLED SPI & control lines
//   ARD      | Port | OLED                | Color
// -----------+------+---------------------|--------
//  #6/D6     |  PD6 | OLED_RESET_NOT      | Purple
//  #7/D7     |  PD7 | VCC_EN              | Blue
//  #8/D8     |  PB0 | OLED_RS             | Yellow
//  #9/D9     |  PB1 | OLED_CS_NOT         | Grey
// #10/D10    |  PB2 | SD_CS_NOT           | Grey
// #11/D11    |  PB3 | MOSI (hardware SPI) | Green
// #12/D12    |  PB4 | MISO (hardware SPI) | Blue
// #13/D13    |  PB5 | SCK  (hardware SPI) | White
//============================================================================

#define SD_CS 10

#define CLR_RESET    (PORTD &= ~(0x40))
#define SET_RESET    (PORTD |=  (0x40))
#define DISABLE_VCC  (PORTD &= ~(0x80))
#define ENABLE_VCC   (PORTD |=  (0x80))

#define CLR_RS       (PORTB &= ~(0x01))
#define SET_RS       (PORTB |=  (0x01))
#define CLR_CS       (PORTB &= ~(0x02))
#define SET_CS       (PORTB |=  (0x02))
#define CLR_SD_CS    (PORTB &= ~(0x04))
#define SET_SD_CS    (PORTB |=  (0x04))
#define CLR_MOSI     (PORTB &= ~(0x08))
#define SET_MOSI     (PORTB |=  (0x08))
#define CLR_SCK      (PORTB &= ~(0x20))
#define SET_SCK      (PORTB |=  (0x20))
//============================================================================
void SPI_sendCommand(uint8_t command)
{
  // Select the OLED's command register
  CLR_RS;
  // Select the OLED controller
  CLR_CS;
  //Send the command via hardware SPI, wait for all bits to clock out:
  SPI.transfer(command);
  // Deselect the OLED controller
  SET_CS;
}
//----------------------------------------------------------------------------
void SPI_sendData(uint8_t data)
{
  // Select the OLED's data register
  SET_RS;
  // Select the OLED controller
  CLR_CS;
  //Send the command via hardware SPI, wait for all bits to clock out:
  SPI.transfer(data);
  // Deselect the OLED controller
  SET_CS;
}
//============================================================================
// Defines for the SSD1331 registers.
// ref: https://www.crystalfontz.com/controllers/SolomonSystech/SSD1331/
#define SSD1331_0x15_Column_Address         (0x15)
#define SSD1331_0x21_Draw_Line              (0x21)
#define SSD1331_0x22_Draw_Rectangle         (0x22)
#define SSD1331_0x23_Copy                   (0x23)
#define SSD1331_0x24_Dim_Window             (0x24)
#define SSD1331_0x25_Clear_Window           (0x25)
#define SSD1331_0x26_Fill_Copy_Options      (0x26)
#define SSD1331_0x27_Scrolling_Options      (0x27)
#define SSD1331_0x2E_Scrolling_Stop         (0x2E)
#define SSD1331_0x2F_Scrolling_Start        (0x2F)
#define SSD1331_0x75_Row_Address            (0x75)
#define SSD1331_0x81_Contrast_A_Blue        (0x81)
#define SSD1331_0x82_Contrast_B_Green       (0x82)
#define SSD1331_0x83_Contrast_C_Red         (0x83)
#define SSD1331_0x87_Master_Current         (0x87)
#define SSD1331_0x8A_Second_Precharge       (0x8A)
#define SSD1331_0xA0_Remap_Data_Format      (0xA0)
#define SSD1331_0xA1_Start_Line             (0xA1)
#define SSD1331_0xA2_Vertical_Offset        (0xA2)
#define SSD1331_0xA4_Mode_Normal            (0xA4)
#define SSD1331_0xA5_Mode_All_On            (0xA5)
#define SSD1331_0xA6_Mode_All_Off           (0xA6)
#define SSD1331_0xA7_Mode_Inverse           (0xA7)
#define SSD1331_0xA8_Multiplex_Ratio        (0xA8)
#define SSD1331_0xAB_Dim_Mode_Setting       (0xAB)
#define SSD1331_0xAD_Master_Configuration   (0xAD)
#define SSD1331_0x8F_Param_Not_External_VCC (0x8F)
#define SSD1331_0x8E_Param_Set_External_VCC (0x8E)
#define SSD1331_0xAC_Display_On_Dim         (0xAC)
#define SSD1331_0xAE_Display_Off_Sleep      (0xAE)
#define SSD1331_0xAF_Display_On_Normal      (0xAF)
#define SSD1331_0xB0_Power_Save_Mode        (0xB0)
#define SSD1331_0x1A_Param_Yes_Power_Save   (0x1A)
#define SSD1331_0x0B_Param_No_Power_Save    (0x0B)
#define SSD1331_0xB1_Phase_1_2_Period       (0xB1)
#define SSD1331_0xB3_Clock_Divide_Frequency (0xB3)
#define SSD1331_0xB8_Gamma_Table            (0xB8)
#define SSD1331_0xB9_Linear_Gamma           (0xB9)
#define SSD1331_0xBB_Precharge_Voltage      (0xBB)
#define SSD1331_0xBE_VCOMH_Voltage          (0xBE)
#define SSD1331_0xFD_Lock_Unlock            (0xFD)
#define SSD1331_0x12_Param_Unlock           (0x12)
#define SSD1331_0x16_Param_Lock             (0x16)  
//============================================================================
void Initialize_OLED(void)
{
  //Reset the OLED controller by hardware
  CLR_RESET;
  delay(1);
  SET_RESET;
  delay(100);

  //Make sure the chip is not in "lock" mode.
  SPI_sendCommand(SSD1331_0xFD_Lock_Unlock);
  SPI_sendCommand(SSD1331_0x12_Param_Unlock);

  //Turn the display off during setup
  SPI_sendCommand(SSD1331_0xAE_Display_Off_Sleep);
  //Set normal display mode (not all on, all off or inverted)
  SPI_sendCommand(SSD1331_0xA4_Mode_Normal);

  //Set column address range 0-95
  SPI_sendCommand(SSD1331_0x15_Column_Address);
  SPI_sendCommand(0);  //column address start
  SPI_sendCommand(95); //column address end
  //Set row address range 0-63
  SPI_sendCommand(SSD1331_0x75_Row_Address);
  SPI_sendCommand(0);  //row address start
  SPI_sendCommand(63); //row address end
  //Set the master current control, based on the version of OLED
  SPI_sendCommand(SSD1331_0x87_Master_Current);
  //Old "0x0E"
#if CFAL9664BFB1
  SPI_sendCommand(6); // 6/16 (60uA of 160uA)
#else //CFAL9664BFB2
  SPI_sendCommand(9); // 9/16 (90uA of 160uA)
#endif

//From BM7 current sweeps + maths
#if CFAL9664BFB1
//Set current level/fraction for each color channel
#if 1
  //Target 100 cd/m*m (normal)
  SPI_sendCommand(SSD1331_0x81_Contrast_A_Blue);
  SPI_sendCommand(184);
  SPI_sendCommand(SSD1331_0x82_Contrast_B_Green);
  SPI_sendCommand(145);
  SPI_sendCommand(SSD1331_0x83_Contrast_C_Red);
  SPI_sendCommand(176);
#else
  //Target 115 cd/m*m (brighter, may reduce life)
  SPI_sendCommand(SSD1331_0x81_Contrast_A_Blue);
  SPI_sendCommand(218);
  SPI_sendCommand(SSD1331_0x82_Contrast_B_Green);
  SPI_sendCommand(169);
  SPI_sendCommand(SSD1331_0x83_Contrast_C_Red);
  SPI_sendCommand(226);
#endif
#else //CFAL9664BFB2
//Set current level/fraction for each color channel
#if 1
  //Target 100 cd/m*m (normal)
  SPI_sendCommand(SSD1331_0x81_Contrast_A_Blue);
  SPI_sendCommand(85);
  SPI_sendCommand(SSD1331_0x82_Contrast_B_Green);
  SPI_sendCommand(46);
  SPI_sendCommand(SSD1331_0x83_Contrast_C_Red);
  SPI_sendCommand(158);
#else
  //Target 115 cd/m*m (brighter, may reduce life)
  SPI_sendCommand(SSD1331_0x81_Contrast_A_Blue);
  SPI_sendCommand(102);
  SPI_sendCommand(SSD1331_0x82_Contrast_B_Green);
  SPI_sendCommand(59);
  SPI_sendCommand(SSD1331_0x83_Contrast_C_Red);
  SPI_sendCommand(199);
#endif
#endif

  //Set pre-charge drive strength/slope
  SPI_sendCommand(SSD1331_0x8A_Second_Precharge);
  SPI_sendCommand(0x61); //A=Blue
  SPI_sendCommand(0x8b); //required token
  SPI_sendCommand(0x62); //B=Green
  SPI_sendCommand(0x8c); //required token
  SPI_sendCommand(0x63); //C=Red

  //Configure the controller for our panel
  SPI_sendCommand(SSD1331_0xA0_Remap_Data_Format);
  SPI_sendCommand(0x70); //check this
  // 0111 0000
  // ffsv wmhi
  // |||| ||||--*0 = Horizontal Increment
  // |||| |||    1 = Vertical Increment
  // |||| |||---*0 = Com order is 0 -> 95
  // |||| ||     1 = Com order is 95 -> 0
  // |||| ||----*0 = Normal Order SA,SB,SC
  // |||| ||     1 = Reverse Order SC,SB,SA
  // |||| |-----*0 = No Left/Right COM swap
  // ||||        1 = Swap Left/Right COM
  // ||||------- 0 = Scan COM0 -> COM63
  // |||        *1 = Scan COM63 -> COM0
  // |||-------- 0 = COM not split
  // ||         *1 = COM split odd/even
  // ||---------*00 = 256 color format
  //             01 = 65K color format #1
  //             10 = 65K color format #2
  //             11 = reserved
  //Start at line 0
  SPI_sendCommand(SSD1331_0xA1_Start_Line);
  SPI_sendCommand(0);
  //No vertical offset
  SPI_sendCommand(SSD1331_0xA2_Vertical_Offset);
  SPI_sendCommand(0);
  //Mux ratio = vertical resolution = 64
  SPI_sendCommand(SSD1331_0xA8_Multiplex_Ratio);
  SPI_sendCommand(63);
  //Reset Master configuration
  SPI_sendCommand(SSD1331_0xAD_Master_Configuration);
  SPI_sendCommand(SSD1331_0x8F_Param_Not_External_VCC);
  //Enable power save mode
  SPI_sendCommand(SSD1331_0xB0_Power_Save_Mode);
  SPI_sendCommand(SSD1331_0x1A_Param_Yes_Power_Save);
  //Set the periods for our OLED
  SPI_sendCommand(SSD1331_0xB1_Phase_1_2_Period);
  SPI_sendCommand(0xF1); // Phase 2 period Phase 1 period
  // 1111 0001
  // tttt oooo
  // |||| ||||-- 1 to 15 = Phase 1 period in DCLK
  // ||||------- 1 to 15 = Phase 2 period in DCLK
  //Set Display Clock Divide Ratio/Oscillator Frequency
  SPI_sendCommand(SSD1331_0xB3_Clock_Divide_Frequency);
  SPI_sendCommand(0xD0);
  // 1101 0000
  // ffff dddd
  // |||| ||||-- Divide 0 => /1, 15 => /16
  // ||||------- Frequency (See Figure 28 in SSD1331 datasheet)
  //             Table shows ~800KHz for 2.7v
  //             Original comment is "0.97MHz"

  //Set pre-charge voltage level 
  SPI_sendCommand(SSD1331_0xBB_Precharge_Voltage);
  //0x3E => 0.5 * Vcc (See Figure 28 in SSD1331 datasheet)
  SPI_sendCommand(0x3E);

  //Set COM deselect voltage level (VCOMH)
  SPI_sendCommand(SSD1331_0xBE_VCOMH_Voltage);
  // 0011 1110
  // --VV VVV-
  // |||| ||||-- reserved = 0
  // |||| |||--- Voltage Code
  // ||            00000 = 0.44 * Vcc (0x00)
  // ||            01000 = 0.52 * Vcc (0x10)
  // ||            10000 = 0.61 * Vcc (0x20)
  // ||            11000 = 0.71 * Vcc (0x30)
  // ||            11111 = 0.83 * Vcc (0x3E)
  // ||--------- reserved = 0
#if CFAL9664BFB1
  // 0.83 * Vcc (0x3E)
  SPI_sendCommand(0x3E);
#else //CFAL9664BFB2
  // 0.71 * Vcc (0x30)
  SPI_sendCommand(0x30);
#endif

#if 0
  //Set default linear gamma / gray scale table
  SPI_sendCommand(SSD1331_0xB9_Linear_Gamma);
#else
  //Set custom gamma / gray scale table
  //Same curve is used for all channels
  //This table is gamma 2.25
  //See Gamma_Calculations.xlsx
  static const uint8_t gamma[32] PROGMEM = {
    0x01,0x01,0x01,0x02,0x02,0x04,0x05,0x07,
    0x09,0x0B,0x0D,0x10,0x13,0x16,0x1A,0x1E,
    0x22,0x26,0x2B,0x2F,0x34,0x3A,0x3F,0x45,
    0x4B,0x52,0x58,0x5F,0x66,0x6E,0x75,0x7D };

  //Send gamma table
  SPI_sendCommand(SSD1331_0xB8_Gamma_Table);
  uint8_t
    i;
  //Send 32 bytes from the table
  for (i = 0; i < 32; i++)
  {
    SPI_sendCommand(pgm_read_byte(&gamma[i]));
  }
#endif

  //Set Master configuration to select external VCC
  SPI_sendCommand(SSD1331_0xAD_Master_Configuration);
  SPI_sendCommand(SSD1331_0x8E_Param_Set_External_VCC);

  //As a more reasonable default, set the recta
  SPI_sendCommand(SSD1331_0x26_Fill_Copy_Options);
  // 0x00 Draw rectangle outline only
  // 0x01 Draw rectangle outline and fill
  SPI_sendCommand(0x01);

  //Clear the screen to 0x00
  SPI_sendCommand(SSD1331_0x25_Clear_Window);
  SPI_sendCommand(0); //X0
  SPI_sendCommand(0); //Y0
  SPI_sendCommand(95); //X0
  SPI_sendCommand(63); //Y0
  //Let it finish.
  delay(1);
  //Turn the display on
  SPI_sendCommand(SSD1331_0xAF_Display_On_Normal);
}
//============================================================================
void putPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
  SPI_sendCommand(SSD1331_0x21_Draw_Line);
  SPI_sendCommand(x);
  SPI_sendCommand(y);

  SPI_sendCommand(x);
  SPI_sendCommand(y);
  //Map 0-255 (8-bit) colors to 0-63 (6-bit)
  SPI_sendCommand(r >> 2);
  SPI_sendCommand(g >> 2);
  SPI_sendCommand(b >> 2);
}
//============================================================================
// From: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t R, uint16_t G, uint16_t B)
{
  uint16_t x = radius;
  uint16_t y = 0;
  int16_t radiusError = 1 - (int16_t)x;

  while (x >= y)
  {
    //11 O'Clock
    putPixel(x0 - y, y0 + x, R, G, B);
    //1 O'Clock
    putPixel(x0 + y, y0 + x, R, G, B);
    //10 O'Clock
    putPixel(x0 - x, y0 + y, R, G, B);
    //2 O'Clock
    putPixel(x0 + x, y0 + y, R, G, B);
    //8 O'Clock
    putPixel(x0 - x, y0 - y, R, G, B);
    //4 O'Clock
    putPixel(x0 + x, y0 - y, R, G, B);
    //7 O'Clock
    putPixel(x0 - y, y0 - x, R, G, B);
    //5 O'Clock
    putPixel(x0 + y, y0 - x, R, G, B);

    y++;
    if (radiusError < 0)
      radiusError += (int16_t)(2 * y + 1);
    else
    {
      x--;
      radiusError += 2 * (((int16_t)y - (int16_t)x) + 1);
    }
  }
}
//============================================================================
void drawLine(uint8_t x0, uint8_t y0,
  uint8_t x1, uint8_t y1,
  uint8_t r, uint8_t g, uint8_t b)
{
  SPI_sendCommand(SSD1331_0x21_Draw_Line);
  SPI_sendCommand(x0);
  SPI_sendCommand(y0);

  SPI_sendCommand(x1);
  SPI_sendCommand(y1);
  //Map 0-255 (8-bit) colors to 0-63 (6-bit)
  SPI_sendCommand(r >> 2);
  SPI_sendCommand(g >> 2);
  SPI_sendCommand(b >> 2);
}
//============================================================================
void drawRectangle(uint8_t x0, uint8_t y0,
  uint8_t x1, uint8_t y1,
  uint8_t r_line, uint8_t g_line, uint8_t b_line,
  uint8_t r_fill, uint8_t g_fill, uint8_t b_fill)
{
  uint8_t
    i;
  //Sort so we are left to right
  if (x1 < x0)
  {
    i = x0;
    x0 = x1;
    x1 = i;
  }
  //Sort so we are bottom to top
  if (y1 < y0)
  {
    i = y0;
    y0 = y1;
    y1 = i;
  }
  //This is a hardware version, it appears to need the coordiantes
  //sorted, and some undocumented delay.
  SPI_sendCommand(SSD1331_0x22_Draw_Rectangle);
  SPI_sendCommand(x0); //X0
  SPI_sendCommand(y0); //Y0
  SPI_sendCommand(x1); //X1
  SPI_sendCommand(y1); //Y1
  //Outline
  SPI_sendCommand(r_line >> 2);
  SPI_sendCommand(g_line >> 2);
  SPI_sendCommand(b_line >> 2);
  //Fill
  SPI_sendCommand(r_fill >> 2);
  SPI_sendCommand(g_fill >> 2);
  SPI_sendCommand(b_fill >> 2);
  //Give the command some time to complete. Empirical.
  //270 fails 275 works 350 should have some margin
  delayMicroseconds(350);
}
//============================================================================
void fillOLED(uint8_t r, uint8_t g, uint8_t b)
{
  drawRectangle(0, 0,
    95, 63,
    r, g, b,
    r, g, b);
}
//============================================================================
//Gradient Bar fill
void gammaGradient()
{
  uint8_t
    x;
  uint8_t
    c;
  for (x = 0; x <= 95; x++)
  {
    //map 0-255 color levels onto 0-95 pixels
    c = (((uint16_t)x * (uint16_t)256) + (uint16_t)128) / (uint16_t)95;
    //Red
    drawLine(x, 0,
      x, 20,
      c, 0, 0);
    //Green
    drawLine(x, 21,
      x, 42,
      0, c, 0);
    //Blue
    drawLine(x, 43,
      x, 63,
      0, 0, c);
  }
}
//============================================================================
// Sets the active window to the entire display (0,0) to (95,63), and the
// current position to 0,0.
void displayHome()
{
  SPI_sendCommand(SSD1331_0x15_Column_Address);
  //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.  
  SPI_sendCommand(0x00);
  //  01011111:  B[6:0] sets the column end   address from 0-95, RESET=95d.  
  SPI_sendCommand(0x5F);

  SPI_sendCommand(SSD1331_0x75_Row_Address);
  //  00000000:  A[5:0] sets the row    start address from 0-63, RESET=00d.
  SPI_sendCommand(0x00);
  //  00111111:  B[5:0] sets the row    end   address from 0-63, RESET=63d.    
  SPI_sendCommand(0x3F);
}
//============================================================================
//Fill screen with vertical color bars using memory writes.
void displayColorBars()
{
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

  displayHome();

  int
    row;
  int
    col;
  for (row = 0; row < 64; row++)
  {
    for (col = 0; col < 96; col++)
    {
      if (col > 84)
      {
        SPI_sendData(BLACK >> 8);
        SPI_sendData(BLACK & 0x00FF);
      }
      else if (col > 72)
      {
        SPI_sendData(BLUE >> 8);
        SPI_sendData(BLUE & 0x00FF);
      }
      else if (col > 60)
      {
        SPI_sendData(RED >> 8);
        SPI_sendData(RED & 0x00FF);
      }
      else if (col > 48)
      {
        SPI_sendData(GREEN >> 8);
        SPI_sendData(GREEN & 0x00FF);
      }
      else if (col > 36)
      {
        SPI_sendData(CYAN >> 8);
        SPI_sendData(CYAN & 0x00FF);
      }
      else if (col > 24)
      {
        SPI_sendData(MAGENTA >> 8);
        SPI_sendData(MAGENTA & 0x00FF);
      }
      else if (col > 12)
      {
        SPI_sendData(YELLOW >> 8);
        SPI_sendData(YELLOW & 0x00FF);
      }
      else
      {
        SPI_sendData(WHITE >> 8);
        SPI_sendData(WHITE & 0x00FF);
      }
    }
  }
}
//============================================================================
//Shows an image from flash, so we can show an image even if there is no
//uSD card installed.
//16-bit-per-pixel image
#include "massey.h"
void showTractor()
{
  uint16_t
    index;
  index = 0;
  uint8_t
    x;
  uint8_t
    y;

  displayHome();

  for (y = 0; y <= 63; y++)
  {
    for (x = 0; x <= 95; x++)
    {
      SPI_sendData(pgm_read_byte(&massey[index]));
      index++;
      SPI_sendData(pgm_read_byte(&massey[index]));
      index++;
    }
  }
}
//============================================================================
void fillDemo()
{
  uint8_t
    i;
  //Fill Demo
  for (i = 0; i < 2; i++)
  {
    fillOLED(0x00, 0x00, 0x00);
    delay(250);
    fillOLED(0x00, 0x00, 0xFF);
    delay(250);
    fillOLED(0x00, 0xFF, 0x00);
    delay(250);
    fillOLED(0x00, 0xFF, 0xFF);
    delay(250);
    fillOLED(0xFF, 0x00, 0x00);
    delay(250);
    fillOLED(0xFF, 0x00, 0xFF);
    delay(250);
    fillOLED(0xFF, 0xFF, 0x00);
    delay(250);
    fillOLED(0xFF, 0xFF, 0xFF);
    delay(250);
  }
}
//============================================================================
void cheesyLineDemo(void)
{
#define MAX_X (95)
#define MAX_Y (63)
#define CTR_X (48)
#define CTR_Y (32)
  //Hardware lines spin way too fast.
#define SLOW  (2)
  uint8_t
    x;
  uint8_t
    y;
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;
  uint8_t
    i;

  r = 0xff;
  g = 0x00;
  b = 0x80;
  for (i = 0; i < 10; i++)
  {
    for (x = 0; x <= MAX_X; x++)
    {
      drawLine(CTR_X, CTR_Y,
        x, 0,
        r++, g--, b += 2);
      delay(SLOW);
    }
    for (y = 0; y <= MAX_Y; y++)
    {
      drawLine(CTR_X, CTR_Y,
        MAX_X, y,
        r++, g += 4, b += 2);
      delay(SLOW);
    }
    for (x = MAX_X; 0 != x; x--)
    {
      drawLine(CTR_X, CTR_Y,
        x, MAX_Y,
        r -= 3, g -= 2, b -= 1);
      delay(SLOW);
    }
    for (y = MAX_Y; 0 != y; y--)
    {
      drawLine(CTR_X, CTR_Y,
        0, y,
        r + -3, g--, b++);
      delay(SLOW);
    }
  }
}
//============================================================================
void checkerDemo(void)
{
  //8x8 checkerboard demo
  uint8_t
    x;
  uint8_t
    y;
  uint8_t
    i;
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;

  //Some seed colors
  r = 17;
  g = 34;
  b = 68;

  for (i = 0; i < 20; i++)
  {
    //Write a 8x8 checkerboard
    for (x = 0; x <= 11; x++)
    {
      for (y = 0; y <= 7; y++)
      {
        if (((x & 0x01) && !(y & 0x01)) || (!(x & 0x01) && (y & 0x01)))
        {
          drawRectangle((x << 3), (y << 3),
            (x << 3) + 7, (y << 3) + 7,
            0, 0, 0,
            0, 0, 0);
        }
        else
        {
          //Come up with some kind of colors
          r = 0xFF - ((x + i) << 4);
          g = 0xFF - ((x << 5) + (i << 3));
          b = 0xFF - (y << 6);
          r++;
          g--;
          b += 2;
          drawRectangle((x << 3), (y << 3),
            (x << 3) + 7, (y << 3) + 7,
            r, g, b,
            r, g, b);
        }
      }
      delay(5);
    }
  }
}
//============================================================================
void circleDemo(void)
{
  uint8_t
    x;
  uint8_t
    i;
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;
  fillOLED(0x00, 0x00, 0x00);
  r = 0xff;
  g = 0x00;
  b = 0x80;
  for (i = 0; i < 80; i++)
  {
    for (x = 4; x < 92; x += 3)
    {
      uint8_t
        rad;
      if (x < 48)
      {
        rad = (x << 1) / 3;
      }
      else
      {
        rad = ((96 - x) << 1) / 3;
      }
      drawCircle(x,
        (x << 1) / 3,
        rad,
        r + -6, g -= 4, b += 11);
    }
  }
}
//============================================================================
#if BUILD_SD
#if(1)
// This function transfers data, in one stream. Slightly
// optimized to do index operations during SPI transfers.
// 312uS ~ 319uS
void SPI_send_pixels(uint8_t pixel_count, uint8_t *data_ptr)
{
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;
  uint8_t
    first_half;
  uint8_t
    second_half;

  // Select the OLED's data register
  SET_RS;
  // Select the OLED controller
  CLR_CS;

  //Load the first pixel. BMPs BGR format
  b = *data_ptr;
  data_ptr++;
  g = *data_ptr;
  data_ptr++;
  r = *data_ptr;
  data_ptr++;

  //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
  //to show one pixel.
  first_half = (r & 0xF8) | (g >> 5);
  second_half = ((g << 3) & 0xE0) | (b >> 3);

  while (pixel_count)
  {
    //Send the first half of this pixel out
    SPDR = first_half;
    //Load the next pixel while that is transmitting
    b = *data_ptr;
    data_ptr++;
    g = *data_ptr;
    data_ptr++;
    r = *data_ptr;
    data_ptr++;
    //Calculate the next first half while that is transmitting
    // ~1.9368us -0.1256 us = 1.8112uS
    first_half = (r & 0xF8) | (g >> 5);
    //This still seems to be needed, but it should exit more quickly.
    while (!(SPSR & _BV(SPIF)));
    //Send the second half of the this pixel out
    SPDR = second_half;
    //Calculate the next second half
    second_half = ((g << 3) & 0xE0) | (b >> 3);
    //Done with this pixel
    pixel_count--;
    //This still seems to be needed, but it should exit more quickly.
    while (!(SPSR & _BV(SPIF)));
  }
  //Wait for the final transfer to complete before we bang on CS.
  while (!(SPSR & _BV(SPIF)));
  // Deselect the OLED controller
  SET_CS;
}
#else
// Simple. Slower
// This function transfers data, in one stream.
// 660uS ~ 667uS
void SPI_send_pixels(uint8_t pixel_count, uint8_t *data_ptr)
{
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;

  //  SPI_sendCommand(LD7138_0x0C_DATA_WRITE_READ);
  while (pixel_count)
  {
    //Load the first pixel. BMPs BGR format
    b = *data_ptr;
    data_ptr++;
    g = *data_ptr;
    data_ptr++;
    r = *data_ptr;
    data_ptr++;

    //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
    //two show one pixel.
    //Calculate the first byte
    //RRRR RGGG
    SPI_sendData((r & 0xF8) | (g >> 5));
    //Calculate the second byte
    //GGGB BBBB
    SPI_sendData(((g << 3) & 0xE0) | (b >> 3));
    //Done with this pixel
    pixel_count--;
  }
}
#endif  
//----------------------------------------------------------------------------
void show_BMPs_in_root(void)
{
  File
    root_dir;
  root_dir = SD.open("/");
  if (0 == root_dir)
  {
    Serial.println("show_BMPs_in_root: Can't open \"root\"");
    return;
  }
  File
    bmp_file;

  while (1)
  {
    bmp_file = root_dir.openNextFile();
    if (0 == bmp_file)
    {
      // no more files, break out of while()
      // root_dir will be closed below.
      break;
    }
    //Skip directories (what about volume name?)
    if (0 == bmp_file.isDirectory())
    {
      //The file name must include ".BMP"
      if (0 != strstr(bmp_file.name(), ".BMP"))
      {
        //The BMP must be exactly 18488 long
        //(this is correct for 96x64, 24-bit)
        if (18488 == bmp_file.size())
        {
          //Jump over BMP header. BMP must be 96x64 24-bit
          bmp_file.seek(54);
#if (0)
          // Simple and very slow          
          uint8_t
            x;
          uint8_t
            y;
          for (y = 0; y <= 63; y++)
          {
            for (x = 0; x <= 95; x++)
            {
              uint8_t
                r;
              uint8_t
                g;
              uint8_t
                b;

              //Read  in the RGB triple for this pixel (BMPs are BGR format)
              bmp_file.read(&b, 1);
              bmp_file.read(&g, 1);
              bmp_file.read(&r, 1);
              //Plot the pixel
              putPixel(x, y, r, g, b);
            }
          }
#else
          //Slightly optimized, reads in one line, then writes those
          //pixels to the OLED.
          displayHome();
          static uint8_t
            rgb_line_pixels[96 * 3];
          for (uint8_t line = 0; line <= 63; line++)
          {
            //Read in one line.
            bmp_file.read(rgb_line_pixels, 96 * 3);
            //Now write this half to the OLED, doing the BGR -> 565
            //color fixup interlaced with the SPI transfers.
            SPI_send_pixels(96, rgb_line_pixels);
          }
#endif
        }
      }
    }
    //Release the BMP file handle
    bmp_file.close();
    delay(1000);
  }
  //Release the root directory file handle
  root_dir.close();
}
#endif
//============================================================================
void setup()
{

  //Set port directions
  DDRB |= 0x2F;
  DDRD |= 0xC0;

  //Drive the ports to a reasonable starting state.
  DISABLE_VCC;
  CLR_RESET;
  CLR_RS;
  SET_CS;
  CLR_MOSI;
  CLR_SCK;

  //debug console
  Serial.begin(115200);
  Serial.println("setup()");

  // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  //Bump the clock to 8MHz. Appears to be the maximum.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //Initialize the OLED controller
  Initialize_OLED();
  delay(50);
  //Turn on the (~14v) OLED power supply
  ENABLE_VCC;
  delay(50);

#if BUILD_SD
  // For the Seeduino I am using, the default speed of SPI_HALF_SPEED
  // set in C:\Program Files (x86)\Arduino\libraries\SD\src\SD.cpp
  // results in a 4MHz clock.
  //
  // If you change this function call in SDClass::begin() of SD.cpp
  // from:
  //
  //  return card.init(SPI_HALF_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // to:
  //
  //  return card.init(SPI_FULL_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // That appears to make the SD library talk at 8MHz.
  //
  if (!SD.begin(SD_CS))
  {
    Serial.println("Card failed to initialize, or not present");
    //Reset the SPI clock to fast. SD card library does not clean up well.
    //Bump the clock to 8MHz. Appears to be the maximum.
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  }
  else
  {
    Serial.println("Card initialized.");
  }
#endif  
}

//============================================================================
// Main Loop
//
// Set/Clear the defines to run a particular demo
// SD must be connected in order for BmpDemo to work
//============================================================================
#define gammagradientdemo 1
#define colorbarsdemo     1
#define tractordemo       1
#define filldemo          1
#define cheesylinesdemo   1
#define checkerdemo       1
#define circledemo        1
#define bmpdemo           0
#define waittime          1000
void loop()
{

 // drawLine(0, 0, 95, 63, 60, 60, 60);
  //drawRectangle(10, 10, 20, 15, 60, 60, 60, 60, 60, 60);
  //while (1);

#if gammagradientdemo
  gammaGradient();
  delay(waittime);
#endif
#if colorbarsdemo
  displayColorBars();
  delay(waittime);
#endif
#if tractordemo
  showTractor();
  delay(waittime);
#endif
#if filldemo
  fillDemo();
#endif
#if cheesylinesdemo
  cheesyLineDemo();
#endif
#if checkerdemo
  checkerDemo();
#endif
#if circledemo
  circleDemo();
#endif
#if bmpdemo
  //Bitmaps demo
  show_BMPs_in_root();
  delay(waittime);
#endif
}
//============================================================================
