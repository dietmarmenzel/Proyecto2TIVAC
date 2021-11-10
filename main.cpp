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