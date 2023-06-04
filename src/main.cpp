#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DS18B20.h>

#define CYCLE_DELAY 1000
#define WARMUP_DELAY 1000
#define DEW_POINT_TRIGGER_VALUE 128

#define PELTIER_COOLDOWN_TEMPERATURE 40.0

#define PIN_RS 8
#define PIN_EN 9
#define PIN_D4 A0
#define PIN_D5 A1
#define PIN_D6 A2
#define PIN_D7 A3

#define PIN_DEW_POINT A4

#define PIN_18b20_WET 2
#define PIN_18b20_DRY 3
#define PIN_18b20_PELTIER 4

#define PIN_PELTIER PD7

// function prototype
float calculateRelativeHumidity(float dryTemperature, float wetTemperature);

LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

sensors_event_t dhtHumEvent;
float relativeHumidity;

DS18B20 ds_wet(PIN_18b20_WET);
DS18B20 ds_dry(PIN_18b20_DRY);
DS18B20 ds_peltier(PIN_18b20_PELTIER);

float dry_temp;
float wet_temp;

void setup () {
  lcd.begin(20, 4);
  lcd.noAutoscroll();
  lcd.setCursor(0, 0);
  lcd.println("init");  
  ds_wet.getTempC();
  ds_dry.getTempC();
  ds_peltier.getTempC();
  pinMode(PIN_PELTIER, OUTPUT);
  pinMode(PIN_DEW_POINT, INPUT);

  _delay_ms(WARMUP_DELAY);
}

// printf("T wet %f", wet_temp); doesnt work in proteus project!
void loop () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("measuring...");

  _delay_ms(CYCLE_DELAY);
  
  digitalWrite(PIN_PELTIER, HIGH);

  uint16_t dewPointSensorValue = 0;
  do {
    lcd.setCursor(0, 1);
    dewPointSensorValue = analogRead(PIN_DEW_POINT);
    lcd.print(dewPointSensorValue);
  } while (dewPointSensorValue > DEW_POINT_TRIGGER_VALUE);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("dew point!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("cooldown peltier...");
  
  digitalWrite(PIN_PELTIER, LOW);

  wet_temp = ds_wet.getTempC();
  dry_temp = ds_dry.getTempC();

  lcd.setCursor(0, 1);
  lcd.print("Tdry: ");
  lcd.print(dry_temp);
  lcd.setCursor(0, 2);
  lcd.print("Twet: ");
  lcd.print(wet_temp);
  lcd.setCursor(0, 3);
  
  relativeHumidity = calculateRelativeHumidity(dry_temp, wet_temp);

  lcd.print("Hrel: ");
  lcd.println(relativeHumidity);

  float pTemp = ds_peltier.getTempC();
  
  while (pTemp > PELTIER_COOLDOWN_TEMPERATURE) {
      _delay_ms(5000);
      pTemp = ds_peltier.getTempC();
  };
  
  _delay_ms(CYCLE_DELAY);
}

/* Слава богу у нас есть нейросети! */
float calculateRelativeHumidity(float dryTemperature, float wetTemperature) {
    float vaporPressure = 6.11 * pow(10, (7.5 * wetTemperature / (237.7 + wetTemperature)));
    float saturationVaporPressure = 6.11 * pow(10, (7.5 * dryTemperature / (237.7 + dryTemperature)));
    float relativeHumidity = (vaporPressure / saturationVaporPressure) * 100;
    return relativeHumidity;
}