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

//*********************************************************************************************************************
//Sensor de Temperatura
//*********************************************************************************************************************
void sensorTemperatura(void){
  //Estado boton 1
  if (digitalRead(BTN1)==0){
    //Suena la nota cuando se apache el boton
    for (int thisNote = 0; thisNote < 4; thisNote++) {
      //Duracion de la nota
      int noteDuration1 = 1000 / duracion2[thisNote];
      tone(bocina, cancion2[thisNote], noteDuration1);
      int pauseBetweenNotes = noteDuration1 * 1.30;
      delay(pauseBetweenNotes);
      noTone(bocina);
      comunicacion = true; 
    }
        delay(10);
    if(digitalRead(BTN1)==1){
      if(comunicacion){
        if (Serial3.available()>0){
          texto1 = Serial3.readStringUntil('\n');
         
        }
        //Despliegue el resultado cuando se apache el boton 1
        Serial3.println(texto1);
        Serial.println(texto1);
        comunicacion = false; 
      }

   }
  } 
}

//*********************************************************************************************************************
//Guardar datos del sensor en SD
//*********************************************************************************************************************
void guardarInfo(void){
 //Estado boton 2
 if (digitalRead(BTN2)==0){
   //Suena la nota cuando se apache el boton
   for (int thisNote = 0; thisNote < 4; thisNote++) {
      //Duración de la nota
      int noteDuration = 1000 / duracion1[thisNote];
      tone(bocina, cancion1[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(bocina);
      subirDatos=true; 
    }
       delay(10);
    if(digitalRead(BTN2)==1){
      //Guardar datos en SD
      if(subirDatos){
        writeSD();  
        subirDatos = false; 
        Serial.println("Se guardaron los datos en la memoria SD"); 
      }
    }
}
}
void writeSD(void) {
  //Archivo donde se encuentre los resultados
  myFile = SD.open("datos.csv", FILE_WRITE);
  if (myFile) {
    Serial.println("Escribiendo datos");
    Serial.print("Temperatura: ");
    Serial.print(texto1);
    myFile.print(texto1.toInt());
    myFile.println(",");

    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening data.csv");
  }
}

//*********************************************************************************************************************
//Lectura de datos
//*********************************************************************************************************************
void lecturaDatos(void){
  texto1 = Serial3.readStringUntil('\n');
  Serial.println(texto1);
}

//*********************************************************************************************************************
//Inicializa la pantalla TFT
//*********************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }

//*********************************************************************************************************************
//Cómo debe ser la inicialización de la pantalla TFT
//*********************************************************************************************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);

  //*********************************************************************************************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
//*********************************************************************************************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
//*********************************************************************************************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
//*********************************************************************************************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
//*********************************************************************************************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
//*********************************************************************************************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
//*********************************************************************************************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
//*********************************************************************************************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
//*********************************************************************************************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
//*********************************************************************************************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
//*********************************************************************************************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
//*********************************************************************************************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
//*********************************************************************************************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//*********************************************************************************************************************
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}

//*********************************************************************************************************************
//Intrucciones para enviar datos a la pantalla TFT
//*********************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}

//*********************************************************************************************************************
//Intrucciones para enviar datos a la pantalla TFT
//*********************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}

//*********************************************************************************************************************
//Parámetros que debe de trabajar de la tarjeta 
//*********************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}

//*********************************************************************************************************************
//Pantalla TFT en blanco
//*********************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}

//*********************************************************************************************************************
//Coordenadas para el dato de la pantalla horizontal
//*********************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}

//*********************************************************************************************************************
//Coordenadas para el dato de la pantalla vertical
//*********************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}