#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define ONE_WIRE_BUS 4
#define WIND_DIR_PWR 10
#define WIND_DIR_PIN A1
#define WIND_SPEED_PWR 9
#define WIND_SPEED_PIN 3

float temp;
float humidity;
float barPressure;              // (barometric pressure)
uint16_t windDirectionDeg;

volatile float windSpeedPulses;
unsigned long windSpeedLastTime;
float windSpeedKPH;
float windSpeedInterval = 10000; 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

Adafruit_BME280 bme280;


void setup() {
  Serial.begin(9600);

  ds18b20.begin();
  bme280.begin(0x76);
  
  
  pinMode(WIND_DIR_PWR, OUTPUT);
  pinMode(WIND_SPEED_PIN, INPUT);
  
  
  pinMode(WIND_SPEED_PWR, OUTPUT);
  
}

void loop() {
  
  
  windSpeedLastTime = millis();
  attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), windspeedInterrupt, RISING);
  digitalWrite(WIND_DIR_PWR, HIGH);
  digitalWrite(WIND_SPEED_PWR, HIGH);
  
  
  
  ds18b20.requestTemperatures();
  temp = ds18b20.getTempCByIndex(0);
  humidity = bme280.readHumidity();
  barPressure = bme280.readPressure() / 100.0F; //hPa
  windDirectionDeg = analogRead(WIND_DIR_PIN);

  
  digitalWrite(WIND_DIR_PWR, LOW);
  
  
  while (millis() < (windSpeedLastTime + windSpeedInterval)); 
  calculateWindSpeed();
  detachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN));
  digitalWrite(WIND_SPEED_PWR, LOW);

  Serial.print("temp: "); Serial.print(temp); Serial.print("  hum: "); Serial.print(humidity); Serial.print("  press: "); Serial.print(barPressure); 
  Serial.print("  wind dir: "); Serial.print(windDirectionDeg); Serial.print("  wind speed: "); Serial.println(windSpeedKPH);

  delay(1000);
}


void windspeedInterrupt() {
  windSpeedPulses++;
}

void calculateWindSpeed(){
    if (windSpeedPulses < 3) {
      windSpeedKPH = 0;
      windSpeedPulses = 0;
      windSpeedLastTime = millis();
    } else {
      windSpeedKPH = (windSpeedPulses / ((millis() - windSpeedLastTime) / 1000)) * 0.88 * 3.6;
      windSpeedPulses = 0;
      windSpeedLastTime = millis();
    }
  }
