#include <ArduinoSTL.h>
#include <Adafruit_BMP085.h>
#include <math.h>
#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

using namespace std;

/*
 * declaración de funciones 
 */
 
vector<int> roundValues(vector<float> v);
vector<int> sortVector(vector<int> a);
int getModa(vector<int> v);

/*
 * variables globales 
 */

Adafruit_BMP085 bmp;

Uart Serial2 (&sercom0, 0, 1, SERCOM_RX_PAD_3, UART_TX_PAD_2);

void SERCOM_Handler()
{
  Serial2.IrqHandler();
}

float seaLevelPressure;
vector<float> floatValues;
int counter;
int currentModa;
byte msg[] ={0, 0};
 
void setup() {
  Serial2.begin(9600);
 
  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(0, PIO_SERCOM);
  pinPeripheral(1, PIO_SERCOM);
  
  if (!bmp.begin()) {
    Serial2.println("Could not find BMP180 or BMP085 sensor");
    while (1) {}
  }
  
  delay(1000);
  
  float suma = 0;
  
  for(int i = 0; i < 25; i ++) {
    suma += bmp.readPressure();
  }
  
  seaLevelPressure = suma / 25;
  counter = 0;
  currentModa = 0;
}
 
void loop() {
  if(counter < 25) {
    float value = bmp.readAltitude(seaLevelPressure);
   
    floatValues.push_back(value);
    counter ++;
  } else {
    vector<int> roundedValues = roundValues(floatValues);
    vector<int> arr_ordenado = sortVector(roundedValues);
    int moda = getModa(arr_ordenado);
 
     if (moda != currentModa) {
       // send data with xbee
      int cociente = moda / 128;
      int resto = moda % 128;

      msg[0] = cociente;
      msg[1] = resto;
      Serial2.write(msg, 2);
      
      currentModa = moda;
    }

      counter = 0;

      // clear values
      floatValues.clear();
      
      delay(300);    
  } 
}

vector<int> roundValues(vector<float> v) {
  vector<int> result(v.size());

  for(int i = 0; i < v.size(); i ++) {
    result[i] = (int)round(v[i]);
  }
  
  return result;
}

vector<int> sortVector(vector<int> a) {
    for(int i = 0; i < (a.size() -1); i++) {
        for(int o = 0; o < (a.size() - (i+1)); o++) {
                if(a[o] > a[o+1]) {
                    int t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                }
        }
    }

    return a;
}

int getModa(vector<int> v) {
  int frecAn = 1;
  int frecAc = 1;
  int moda = v[0];
 
  for(int i = 1; i <= v.size(); i ++) {
   if(v[i] == v[i -1]) {
     frecAc +=1;
   } else {
     if (frecAc > frecAn) {
      frecAn = frecAc;
      moda = v[i - 1];
     }
     frecAc = 1;
   }
  }

  return moda;
}
