

#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

int pHSense = A0;
int samples = 10;
float adc_resolution = 1024.0;

#define WIFI_SSID "Qual a senha do wifi?" // Nome da rede WiFi.
#define WIFI_PASSWORD "Farofa123" // Senha da rede WiFi.

#define API_KEY "AIzaSyAqdzU2tHHXmFe9B8Ww7HHVwbfI2ftax2s"
#define DATABASE_URL "https://sensor-turbidez-default-rtdb.firebaseio.com"


void setup()
{
    Serial.begin(9600);
    delay(100);
    Serial.println("cimpleo pH Sense");
}

float ph (float voltage) {
    return 7 + ((2.5 - voltage) / 0.18);
}

void loop()
{
    int measurings=0;

    for (int i = 0; i < samples; i++)
    {
        measurings += analogRead(pHSense);
        delay(10);

    }

    float voltage = 5 / adc_resolution * measurings/samples;
    Serial.print("pH= ");
    Serial.println(ph(voltage));
    delay(3000);
}
