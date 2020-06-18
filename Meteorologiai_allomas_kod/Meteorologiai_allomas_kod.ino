/* Az időjárás állomásban lévő Arduino Nano programkódja*/
/* Tósoki Tibor 2020*/


#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "LowPower.h"


#define ONE_WIRE_BUS 4
#define WIND_DIR_PWR 10
#define WIND_SPEED_PWR 9
#define WIND_DIR_PIN A1
#define WIND_SPEED_PIN 3
// Ennyi mérést végzünk (a tömbök elemszáma).
#define NUM_OF_READINGS 9

// struktúra
struct WeatherDatas {
  float temp;
  float humidity;
  float barPressure;
  float windSpeedKPH;
  uint16_t windDirDeg;
};
WeatherDatas weatherDatas;


volatile uint16_t windSpeedPulses;
// a szélsebességmérés kezdete
unsigned long windSpeedStartTime;
// szélsebességmérés időtartama
float windSpeedInterval = 60000;
uint16_t windDirAnalogReading;

float temperatures[NUM_OF_READINGS];
float humidities[NUM_OF_READINGS];    
float barPressures[NUM_OF_READINGS];
uint16_t windDirAnalogReadings[NUM_OF_READINGS];

RF24 radio(2, 6);
const byte address[6] = "00001";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

Adafruit_BME280 bme280;



void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();

  ds18b20.begin(); 
  bme280.begin(0x76);

  pinMode(WIND_DIR_PWR, OUTPUT);
  pinMode(WIND_SPEED_PWR, OUTPUT);
  pinMode(WIND_DIR_PIN, INPUT);
  pinMode(WIND_SPEED_PIN, INPUT); 
}

// megszakítás kiszolgáló rutin (ISR)
void windspeedInterrupt() {   			
  windSpeedPulses++;
}


void loop() {
  attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), windspeedInterrupt, RISING);
  digitalWrite(WIND_SPEED_PWR, HIGH);
  windSpeedStartTime = millis();

  readSensors();

  weatherDatas.temp = average(temperatures);
  weatherDatas.humidity = average(humidities);
  weatherDatas.barPressure = average(barPressures);
  windDirAnalogReading = median(windDirAnalogReadings);
  
  weatherDatas.windDirDeg = map(windDirAnalogReading, 0, 1023, 0, 359);

  while (millis() < (windSpeedStartTime + windSpeedInterval));                   					
  detachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN)); 		
  digitalWrite(WIND_SPEED_PWR, LOW);
  calculateWindSpeed();     

  radio.write(&weatherDatas, sizeof(weatherDatas));

  // Megvárjuk, míg biztosan befejeződik a küldés.
  delay(1000);
  sleep();
}



void calculateWindSpeed() {
	// 1 fordulat / mp = 0.88 m/s
    weatherDatas.windSpeedKPH =                                // (átváltás km/h-ba)
          (windSpeedPulses / ((millis() - windSpeedStartTime) / 1000)) * 0.88 * 3.6;
    windSpeedPulses = 0;
}



void readSensors() {
	// Szélirányszenzor bekapcsolása.
  digitalWrite(WIND_DIR_PWR, HIGH);

  for (int i = 0; i < NUM_OF_READINGS; i++) {
    ds18b20.requestTemperatures();
    temperatures[i] = ds18b20.getTempCByIndex(0);
    humidities[i] = bme280.readHumidity();
    barPressures[i] = bme280.readPressure() / 100.0F;
    windDirAnalogReadings[i] = analogRead(WIND_DIR_PIN);
    delay(1000);
  }
  digitalWrite(WIND_DIR_PWR, LOW);
}

// számtani átlag
float average(float *array) {
  float sum = 0;
  for (int i = 0 ; i < NUM_OF_READINGS ; i++) {
    sum += array [i] ;
  }
  return  sum / NUM_OF_READINGS ;
}


uint16_t median(uint16_t *array) {
  // buborékrendezés
  for (int x = 0; x < NUM_OF_READINGS; x++)
    for (int y = 0; y < NUM_OF_READINGS - 1; y++)
      if (array[y] > array[y + 1]) {
        uint16_t holder = array[y + 1];
        array[y + 1] = array[y];
        array[y] = holder;
      }
  // "középső" elem
  return array[NUM_OF_READINGS / 2];
}


// 10 perc - 1 perc (szélsebességmérés hossza) - 1 mp (delay) = 8 perc 59 mp = 539 mp
void sleep() {
  // Csak maximum 8 mp-es alvást indíthatunk, ezért ezeket halmozzuk
  // 536mp
  for (int i = 0; i < 67; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  // +3mp
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
}
