#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DS18B20.h>

#define RESULT_DELAY 10000
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
  lcd.begin(16, 2);
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

  
  digitalWrite(PIN_PELTIER, LOW);

  wet_temp = ds_wet.getTempC();
  dry_temp = ds_dry.getTempC();
  float pTemp = ds_peltier.getTempC();
  relativeHumidity = calculateRelativeHumidity(dry_temp, wet_temp);

  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Tw:");
  lcd.print(wet_temp);
  lcd.print(" Td:");
  lcd.print(dry_temp);
  lcd.setCursor(0, 0);
  lcd.print("tp:");
  lcd.print(pTemp);
  lcd.print(" rH:");
  lcd.print(relativeHumidity);
  
  

  delay(RESULT_DELAY);

  while (pTemp > PELTIER_COOLDOWN_TEMPERATURE) {
      lcd.setCursor(0, 0);
      lcd.print("tp:");
      lcd.print(pTemp);
      lcd.print(" rH:");
      lcd.print(relativeHumidity);
      pTemp = ds_peltier.getTempC();
      _delay_ms(5000);
  };
  
  _delay_ms(CYCLE_DELAY);
}

/* Слава богу у нас есть нейросети! */
float calculateRelativeHumidity(float t0, float tp) {
    return 6.112*exp((17.67*tp)/(243.5+tp))/(6.112*exp((17.67*t0)/(243.5+t0)))*100;  
} 