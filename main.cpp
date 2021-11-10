//*********************************************************************************************************************
//Universidad Del Valle de Guatemala
//BE3015 - Electrónica Digital 2
//Gunther Dietmar Menzel - 19106
//PROYECTO 2
//Parte de la Tiva C
//*********************************************************************************************************************

//*********************************************************************************************************************
//Librerías
//*********************************************************************************************************************
#include <Arduino.h>
//Librería SPI para conectarse con la SD
#include <SPI.h>
//Librería SD 
#include <SD.h>
//Librería pantalla TFT
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include <avr/pgmspace.h>
#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

//*********************************************************************************************************************
//Definición de pines
//*********************************************************************************************************************
//Definición de pines para los botones
#define BTN1 PF_0 
#define BTN2 PF_4
//Definición de pines para la bocina
#define bocina PF_3
//Definición de las notas utilizadas
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
//Definición de pines para la pantalla TFT 
#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

//*********************************************************************************************************************
//Prototipos de funciones
//*********************************************************************************************************************
void sensorTemperatura(void); //Función del sensor de temperatura
void guardarInfo(void); //Función para guardar la información de la SD
//Funciones para accionar la pantalla TFT
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//*********************************************************************************************************************
//Variables Globales
//*********************************************************************************************************************
//Fondo de la pantalla TFT 
extern uint8_t fondo[];
//Tiempos de muestra (Delay)
long LastTime1;
long sampleTime1 =15; 
long LastTime2; 
long sampleTime2 = 1000; 
//Notas para la bocina
int cancion1[] = { NOTE_F7, NOTE_F6,NOTE_A5, NOTE_F7  }; 
int cancion2[] = { NOTE_B0, NOTE_A7, NOTE_A7, NOTE_D5};
//Duración de notas para la bocina
int duracion1[] = {1, 4, 8, 8}; // int duracion1[] = {4, 16, 8, 8};
int duracion2[] = {4, 8, 4, 8};
int numero =0;
//Datos del sensor de temperatura
String texto1=""; 
String frase1 = "Sensor de temperatura";
bool comunicacion = false; 
bool subirDatos = false; 
//Archivo para guardar datos
File myFile;

//*********************************************************************************************************************
//Configuración
//*********************************************************************************************************************
void setup() {
  //Comunicación serial
  Serial.begin(115200);
  Serial3.begin(115200);
  //Estado de botones
  pinMode(BTN1, INPUT_PULLUP); 
  pinMode(BTN2, INPUT_PULLUP);

  LastTime1=millis(); 
  LastTime2=millis();
  //Módulo SPI
  SPI.setModule(0);
  //Comunicación SD
  if (!SD.begin(PA_3)) {
    //Si no hay SD insertada
    Serial.println("Error");
    return;
     }
  //Comienza TFT
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

  LCD_Init();
  LCD_Clear(0x00);

  LCD_Bitmap(0, 0, 320, 240, fondo);
}

//*********************************************************************************************************************
//Loop principal
//*********************************************************************************************************************
void loop()
{
 //Posicion del texto en la TFT
 LCD_Print(texto1 ,110, 110, 1, 0x0000,   0xFFFF);
  
  sensorTemperatura();   
  guardarInfo(); 
}

