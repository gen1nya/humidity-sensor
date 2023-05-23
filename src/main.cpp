#include "lcd.h"
#include "lcd.c"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "DHT.h"
#include "DHT.c"

#include <ds18b20/ds18b20.h>

int main() {
  lcd_init();
  lcd_clear();
  lcd_set_cursor(0, 0);
  lcd_printf("ti pidor");
  _delay_ms(2500);

  int16_t dry_temp;
  int16_t wet_temp;

  double dht_temperature[1];
	double dht_humidity[1];

	DHT_Setup();
  
  while(true) {  
		ds18b20convert(&PORTB, &DDRB, &PINB, ( 1 << 0 ), 0);
		_delay_ms(1000);
		ds18b20read(&PORTB, &DDRB, &PINB, ( 1 << 0 ), 0, &dry_temp);
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_printf("t1: %d", (float) dry_temp / 16);

	  ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), 1);
		_delay_ms(1000);
		ds18b20read(&PORTB, &DDRB, &PINB, ( 1 << 0 ), 1, &wet_temp);
    lcd_set_cursor(0, 1);
    lcd_printf("t2: %d", (float) wet_temp / 16);

    DHT_Read(dht_temperature, dht_humidity);
		
  	switch (DHT_GetStatus())	{
			case (DHT_Ok):
        _delay_ms(1000);
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_printf("t3: %d, h1: %d", dht_temperature[0], dht_humidity[0]);
				break;
			case (DHT_Error_Checksum):
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_printf("dht checksum error");
				break;
			case (DHT_Error_Timeout):
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_printf("dht timeout error");
				break;
      default:
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_printf("dht unknown");
        break;
		}
				
		//Sensor needs 1-2s to stabilize its readings
		_delay_ms(1000);
  }

  return 0;

}