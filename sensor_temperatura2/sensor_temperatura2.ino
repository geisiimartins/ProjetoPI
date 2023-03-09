//Sensor de Temperatura - DS18B20

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire onewire(4);
DallasTemperature sensor(&onewire);
float temperatura;

void setup() {
  Serial.begin(9600);
  sensor.begin();

}
void loop() {
  sensor.requestTemperatures();
  temperatura = sensor.getTempCByIndex(0);
  Serial.println(temperatura);
  delay(100);

}
