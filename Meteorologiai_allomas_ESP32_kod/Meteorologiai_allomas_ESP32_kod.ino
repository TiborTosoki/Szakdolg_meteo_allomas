/* A benti egységben lévő ESP32 programkódja */
/* Tósoki Tibor 2020*/


#include <nRF24L01.h>
#include <RF24.h>
#include "DHTesp.h"
#include <WiFi.h>
#include <HTTPClient.h>

// a DHT11 hőszenzor adatlába
#define DHTPIN 15
// Nextion kijelző frissítésének gyakorisága (ms).
#define NEXTION_UPDATE_INTERVAL 5000
// Ha nem jön kintről üzenet fél órán át, hibaüzenet jelenik meg.
#define RADIO_ERROR_INTERVAL 1.8E+6
#define WIFI_SSID "Tosoki"
#define WIFI_PASSWORD ""
#define SERVERNAME "http://idojaras.szakdolgozat.net/PostWeatherData.php"

String apiKey = "zP2kCAxc9YJ5YotgF9On7ytwXLXe4JXb";

RF24 radio(22, 21);
const byte address[6] = "00001";

DHTesp dht;

long lastNextionUpdate;
long lastRadioReceived;
bool isRadioError = false;

struct WeatherDatas {
  float temp;
  float humidity;
  float barPressure;
  float windSpeedKPH;
  uint16_t windDirDeg;
};
WeatherDatas weatherDatas;

// benti hőm.
float inTemp;


void setup() {
  Serial.begin(9600);

  dht.setup(DHTPIN, DHTesp::DHT11);

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
}


void loop() {
  if (radio.available()) {
    radio.read(&weatherDatas, sizeof(weatherDatas));
    isRadioError = false;
    // legutóbbi üzenet idejének rögzítése
    lastRadioReceived = millis();
    uploadToWeb();
  }


  if (millis() - lastRadioReceived > RADIO_ERROR_INTERVAL) {
    isRadioError = true;
  }
  

  if (millis() - lastNextionUpdate > NEXTION_UPDATE_INTERVAL) {
    inTemp = dht.getTemperature(); 
    sendToNextion();
    lastNextionUpdate = millis();
  }
}



void uploadToWeb() {
	
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(SERVERNAME);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "client_api_key=" + apiKey
                             + "&temperature=" + String(weatherDatas.temp)
                             + "&humidity=" + String(weatherDatas.humidity)
                             + "&pressure=" + String(weatherDatas.barPressure)
                             + "&windSpeed=" + String(weatherDatas.windSpeedKPH)
                             + "&windDirectionDegree=" + String(weatherDatas.windDirDeg)
                             + "&windDirectionCompass=" + calcWindDirCompass(weatherDatas.windDirDeg) + "";

	// A weboldal POST metódusának hívása az adatsorral
    http.POST(httpRequestData);
    http.end();
  }
}



void sendToNextion() {
  String command;

  command = "Main.OutTemp.txt=\"" + String(weatherDatas.temp) + "\"";
  Serial.print(command);
  // A különböző üzeneteket három, speciális ANSI karaktert tartalmazó bájttal kell elkülöníteni egymástól.
  endNextionCommand();

  command = "Main.OutTemp.txt=\"" + String(weatherDatas.temp) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "Main.InTemp.txt=\"" + String(inTemp) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "More.Humidity.txt=\"" + String(weatherDatas.humidity) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "More.Pressure.txt=\"" + String(weatherDatas.barPressure) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "More.WindDir.txt=\"" + String(weatherDatas.windDirDeg) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "More.WindDirComp.txt=\"" + calcWindDirCompass(weatherDatas.windDirDeg) + "\"";
  Serial.print(command);
  endNextionCommand();

  command = "More.WindSpeed.txt=\"" + String(weatherDatas.windSpeedKPH) + "\"";
  Serial.print(command);
  endNextionCommand();

  if (isRadioError) {
    Serial.print("ErrorScroll.txt=\" Hiba! 30 perce nincs friss adat!\"");
    endNextionCommand();
  } else {
	// Ha esetleg már írtunk ki hibaüzenetet, de azóta jött új adat, akkor üressé is kell tennünk a lbale tartalmát. 
    Serial.print("ErrorScroll.txt=\"\"");
  }
}


// A fokban vett szélirány átváltása égtájakra. 
String calcWindDirCompass(uint16_t windDirDeg) {
  String WindDirComp;

  if (windDirDeg < 360 / 16)
    WindDirComp = "É";
  else if (windDirDeg < 360 / 16 * 3)
    WindDirComp = "ÉK";
  else if (windDirDeg < 360 / 16 * 5)
    WindDirComp = "K";
  else if (windDirDeg < 360 / 16 * 7)
    WindDirComp = "DK";
  else if (windDirDeg < 360 / 16 * 9)
    WindDirComp = "D";
  else if (windDirDeg < 360 / 16 * 11)
    WindDirComp = "DNY";
  else if (windDirDeg < 360 / 16 * 13)
    WindDirComp = "NY";
  else if (windDirDeg < 360 / 16 * 15)
    WindDirComp = "ÉNY";
  else
    WindDirComp = "É";

  return WindDirComp;
}


void endNextionCommand() {
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}
