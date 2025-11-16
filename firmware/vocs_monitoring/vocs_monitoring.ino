#include <FastLED.h>
#include "Adafruit_SGP40.h"
#include "Adafruit_SHT4x.h"

// LED constraints
#define LED_PIN 12
CRGB leds[1]; // GRB

// Sensirion SHT40 Temperature and Humidity Sensor
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
sensors_event_t humidity, temp;

// Sensirion VOC Sensor
Adafruit_SGP40 sgp;
uint16_t sraw;
int32_t voc_index;

// Utils
#define INTERVAL 1000 // [ms] 
uint32_t timestamp;
bool blink_flag = 0;

void setup() {  

  Serial.begin(115200);

  // Initialize LED
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, 1);

  // Initialize VOC sensor
  if (! sgp.begin()){
    Serial.println("SGP40 sensor not found :(");
    while (1);
  }

  // Initialize temperature sensor
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }

  sht4.setPrecision(SHT4X_HIGH_PRECISION); 
  sht4.setHeater(SHT4X_NO_HEATER);

  // LED Check
  leds[0] = 0xAA0000;
  FastLED.show();
  delay(1000);

  leds[0] = 0x00AA00; 
  FastLED.show();
  delay(1000);

  leds[0] = 0x0000AA; 
  FastLED.show();
  delay(1000);

  leds[0] = 0x000000; 
  FastLED.show();

}

void loop() {

  // Measure humidity, temperature and voc after specifed interval
  if (millis() - timestamp > INTERVAL){

    sht4.getEvent(&humidity, &temp);

    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

    sraw = sgp.measureRaw(temp.temperature, humidity.relative_humidity);
    Serial.print("Raw measurement: "); Serial.println(sraw);

    voc_index = sgp.measureVocIndex(temp.temperature, humidity.relative_humidity);
    Serial.print("Voc Index: "); Serial.println(voc_index);

    timestamp = millis();

    // Change color of LED according to VOC index scale 
    if (voc_index == 0){
      if(blink_flag){
        leds[0] = 0xAA0000; //GRB
        blink_flag = 0;
      }else{
        leds[0] = 0x000000;
        blink_flag = 1;
      }
    }else if(voc_index < 70){
        leds[0] = 0x0000AA;
    }else if(voc_index < 140){
        leds[0] = 0xAA0000;
    }else if(voc_index < 210){
        leds[0] = 0xAAAA00;
    }else if(voc_index < 280){
        leds[0] = 0x55AA00;
    }else if(voc_index < 350){
        leds[0] = 0x00AA00;
    }else{
      if(blink_flag){
        leds[0] = 0x00AA00; //GRB
        blink_flag = 0;
      }else{
        leds[0] = 0x000000;
        blink_flag = 1;
      }
    }
  }
  FastLED.show();
}
  

