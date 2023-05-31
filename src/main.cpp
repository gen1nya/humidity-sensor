#include <LiquidCrystal.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DS18B20.h>

#define CYCLE_DELAY 1000
#define WARMUP_DELAY 100
#define PIN_RS 8
#define PIN_EN 9
#define PIN_D4 10
#define PIN_D5 11
#define PIN_D6 12
#define PIN_D7 13
#define PIN_18b20_WET 3
#define PIN_18b20_DRY 4
#define PIN_DHT 2 
#define DHTTYPE    DHT11

// function prototype
float calculateRelativeHumidity(float dryTemperature, float wetTemperature);

LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

DHT_Unified dht(PIN_DHT, DHTTYPE);

sensors_event_t dhtTempEvent;
sensors_event_t dhtHumEvent;

DS18B20 ds_wet(PIN_18b20_WET);
DS18B20 ds_dry(PIN_18b20_DRY);

float dry_temp;
float wet_temp;

void setup () {
  lcd.begin(20, 4);
  lcd.noAutoscroll();
  lcd.setCursor(0, 0);
  lcd.println("init");  
  
  dht.begin();
  
  lcd.setCursor(0, 1);
  lcd.println("waiting for data");
  
  _delay_ms(WARMUP_DELAY);
}

// printf("T wet %f", wet_temp); doesnt work in proteus project!
void loop () {

  // read 18b20 sensors
  wet_temp = ds_wet.getTempC();
  dry_temp = ds_dry.getTempC();
  
  // read DHT sensor
  dht.temperature().getEvent(&dhtTempEvent);
  dht.humidity().getEvent(&dhtHumEvent);

  // print all data
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(F("T dry: "));
  lcd.println(dry_temp);  

  lcd.setCursor(0, 1);
  lcd.print(F("T wet: "));
  lcd.print(wet_temp);
  
  lcd.setCursor(0, 2);
  lcd.print(F("T DHT: "));
  lcd.print(dhtTempEvent.temperature);
  
  lcd.setCursor(0, 3);
  lcd.print(F("H DHT: "));
  lcd.print(dhtHumEvent.relative_humidity);

  _delay_ms(CYCLE_DELAY);

  float relativeHumidity = calculateRelativeHumidity(dry_temp, wet_temp);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("T dry: "));
  lcd.println(dry_temp);  

  lcd.setCursor(0, 1);
  lcd.print(F("T wet: "));
  lcd.print(wet_temp);

  lcd.setCursor(0, 2);
  lcd.print(F("raletive: "));
  lcd.print(relativeHumidity);
  
  _delay_ms(CYCLE_DELAY);
}

/* Слава богу у нас есть нейросети! */
float calculateRelativeHumidity(float dryTemperature, float wetTemperature) {
    float vaporPressure = 6.11 * pow(10, (7.5 * wetTemperature / (237.7 + wetTemperature)));
    float saturationVaporPressure = 6.11 * pow(10, (7.5 * dryTemperature / (237.7 + dryTemperature)));
    float relativeHumidity = (vaporPressure / saturationVaporPressure) * 100;
    return relativeHumidity;
}