#include <LiquidCrystal.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DS18B20.h>

#define CYCLE_DELAY 100
#define WARMUP_DELAY 100
#define PIN_RS 8
#define PIN_EN 9
#define PIN_D4 10
#define PIN_D5 11
#define PIN_D6 12
#define PIN_D7 13
#define PIN_18b20_WET 3
#define PIN_18b20_DRY 4
#define PIN_DHT 1 
#define PIN_LED 5
#define PIN_MODE_BUTTON 2
#define DHTTYPE DHT11

#define HUMIDITY_MODE 1
#define TEMPERATURE_MODE 0

// function prototype
float calculateRelativeHumidity(float dryTemperature, float wetTemperature);
void changeMode();

LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

DHT_Unified dht(PIN_DHT, DHTTYPE);

sensors_event_t dhtHumEvent;
float relativeHumidity;

DS18B20 ds_wet(PIN_18b20_WET);
DS18B20 ds_dry(PIN_18b20_DRY);

float dry_temp;
float wet_temp;

uint8_t mode = TEMPERATURE_MODE;

void setup () {
  lcd.begin(20, 4);
  lcd.noAutoscroll();
  lcd.setCursor(0, 0);
  lcd.println("init");  
  
  dht.begin();

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_MODE_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_MODE_BUTTON), changeMode, FALLING);
  
  lcd.setCursor(0, 1);
  lcd.println("waiting for data");
  
  _delay_ms(WARMUP_DELAY);
  digitalWrite(PIN_LED, HIGH);
}

// printf("T wet %f", wet_temp); doesnt work in proteus project!
void loop () {
  
  switch (mode) {
    case TEMPERATURE_MODE:  
      // read 18b20 sensors
      wet_temp = ds_wet.getTempC();
      dry_temp = ds_dry.getTempC();

      // print temperature
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("T dry: "));
      lcd.println(dry_temp);  

      lcd.setCursor(0, 1);
      lcd.print(F("T wet: "));
      lcd.print(wet_temp);

    break;

    case HUMIDITY_MODE:
      // read humidity from DHT sensor
      dht.humidity().getEvent(&dhtHumEvent);
      // calculate relative humidity
      float relativeHumidity = calculateRelativeHumidity(dry_temp, wet_temp);

      // print humidity
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("H DHT: "));
      lcd.print(dhtHumEvent.relative_humidity);
      lcd.setCursor(0, 1);
      lcd.print(F("Relative: "));
      lcd.print(relativeHumidity);
  
    break;
  }

  _delay_ms(CYCLE_DELAY);
}

/* Слава богу у нас есть нейросети! */
float calculateRelativeHumidity(float dryTemperature, float wetTemperature) {
    float vaporPressure = 6.11 * pow(10, (7.5 * wetTemperature / (237.7 + wetTemperature)));
    float saturationVaporPressure = 6.11 * pow(10, (7.5 * dryTemperature / (237.7 + dryTemperature)));
    float relativeHumidity = (vaporPressure / saturationVaporPressure) * 100;
    return relativeHumidity;
}

/**
 * external interrup; won't work correctly in the real world (noise)
*/
void changeMode() {
  if (mode == HUMIDITY_MODE) {
    mode = TEMPERATURE_MODE;
    digitalWrite(PIN_LED, HIGH);
  } else {
    mode = HUMIDITY_MODE;
    digitalWrite(PIN_LED, LOW);
  }
}