#include <OneWire.h>
#include <DallasTemperature.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const int NUM_NETWORKS = 5;  // número de redes Wi-Fi
const char* ssids[NUM_NETWORKS] = {"Aquality", "Micks Iot" ,"Qual a senha do wifi?", "Ana Soares", "Maciel"};
const char* passwords[NUM_NETWORKS] = {"12345678", "micks2023" ,"Farofa123", "jesusefielsim", "maciel777"};

#define API_KEY "AIzaSyAqdzU2tHHXmFe9B8Ww7HHVwbfI2ftax2s"
#define DATABASE_URL "https://sensor-turbidez-default-rtdb.firebaseio.com"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

enum LedPin {
  RED_LED = 13,   // D7
  GREEN_LED = 12, // D6
  BLUE_LED = 14   // D5
};

// check esp8266 pinout here
// https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?w=817&quality=100&strip=all&ssl=1

#define turbidityPin A0
#define temperaturePin 4 // D2 

OneWire oneWire(temperaturePin);
DallasTemperature sensors(&oneWire);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

struct MinMax {
  float min;
  float max;
};

MinMax turbidityThreshold = {700, 1024};
MinMax temperatureThreshold = {20, 30};

unsigned long lastThresholdUpdate;
unsigned long lastParameterUpdate;
float temperature;
float turbidity;
float ph;
String dateTime;

void setup() {
  Serial.begin(9600);
  sensors.begin();

  pinMode(RED_LED, HIGH);  
  pinMode(GREEN_LED, HIGH);
  pinMode(BLUE_LED, HIGH);

  initWiFi();
  connectFirebase();

  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
    timeClient.setTimeOffset(-10800); // -10800 => GMT -3
  }

  setParametersThreshold(); 
  lastParameterUpdate = millis();
}

void blinkLed(const LedPin led, const int times = 5, const int durationOn = 100, const int durationOff = 100, const bool stayOn = false) {
  
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  for (int i = 0; i < times; i++) {
    digitalWrite(led, HIGH);
    delay(durationOn);
    digitalWrite(led, LOW);
    delay(durationOff);
  }

  if (stayOn) {
    digitalWrite(led, HIGH);
  }

}

void loop() {
  
  if (WiFi.status() != WL_CONNECTED) {
    blinkLed(RED_LED, 3, 50, 50, true);
  } else {
    blinkLed(GREEN_LED, 0, 50, 50, true);
  }

  if ( millis() - lastThresholdUpdate > 30000 ) {
    setParametersThreshold(); 
  }

  if ( millis() - lastParameterUpdate > 1000 ) {
    readTurbiditySensor();
    readPhSensor();
    readTemperatureSensor();
    lastParameterUpdate = millis();
  }
}


void timeStamp( ) {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);  
  dateTime = String(ptm->tm_mday) + "-" + String(ptm->tm_mon+1) +  "-" + String( ptm->tm_year+1900);
  dateTime += " " + timeClient.getFormattedTime().substring(0, 2) + ":" + timeClient.getFormattedTime().substring(3, 5);
}


void readTurbiditySensor() {

  if ( analogRead(turbidityPin) != turbidity ) {
    sendFloatDataToFirebase("/agua/turbidez/valor", turbidity);
  }

  turbidity = analogRead(turbidityPin);

  Serial.print("Turbidity: ");
  Serial.println(turbidity);

  if ( turbidity >= turbidityThreshold.max || turbidity <= turbidityThreshold.min ) {
    sendAlert();
    Serial.println("Turbidity min :" + String(turbidityThreshold.min) + " max :" + String(turbidityThreshold.max));
    Serial.println("Turbidity outside defined limits. Sending alert");
  }
}


void readTemperatureSensor() {
  sensors.requestTemperatures(); 

  if ( sensors.getTempCByIndex(0) != temperature ) {
    sendFloatDataToFirebase("/agua/temperatura/valor", temperature);
  }

  temperature = sensors.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if ( temperature >= temperatureThreshold.max || temperature <= temperatureThreshold.min ) {
    sendAlert();
    Serial.println("Temperature min :" + String(temperatureThreshold.min) + " max :" + String(temperatureThreshold.max));
    Serial.println("Temperature outside defined limits. Sending alert");
  }
}

static String temp;
void readPhSensor() {
  
  while (Serial.available()) {
    char c = Serial.read();

    if (c == ';') {
      float phValue = temp.toFloat();
      temp = ""; // Limpar a string temporária

      if (!isnan(phValue)) { // Verifica se o valor é um número válido
        if (phValue >= 1 && phValue <= 14) {
          ph = phValue;
          sendFloatDataToFirebase("/agua/ph/valor", ph);
        }
      }
      break;
    } else {
      temp += c;
    }
  }
  
  Serial.println("Ph: " + String(ph));
}


void sendAlert() {
  timeStamp();

  sendFloatDataToFirebase(String("/alertas/" + dateTime + "/ph"), ph);
  sendFloatDataToFirebase(String("/alertas/" + dateTime + "/temperatura"), temperature);
  sendFloatDataToFirebase(String("/alertas/" + dateTime + "/turbidez"), turbidity);

  blinkLed(RED_LED, 3, 500, 100, true);
}


void sendFloatDataToFirebase(const String path, const float data ){
  Firebase.RTDB.setFloat(&fbdo, path, data); 
}


void sendStringDataToFirebase(const String path, const String data ){
  Firebase.RTDB.setString(&fbdo, path, data); 
}


void setParametersThreshold() {
  temperatureThreshold.max = getDataFromFirebaseField("/agua/temperatura/max");
  temperatureThreshold.min = getDataFromFirebaseField("/agua/temperatura/min");

  turbidityThreshold.max = getDataFromFirebaseField("/agua/turbidez/max");
  turbidityThreshold.min = getDataFromFirebaseField("/agua/turbidez/min");

  Serial.println("Parameters Threshold updated");

  lastThresholdUpdate = millis();
}


float getDataFromFirebaseField(const char* path) {

  float value;
 
  if (Firebase.RTDB.getFloat(&fbdo, path)) {
    value = fbdo.to<float>();
  } else {
    Serial.print("Error getting float value: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  return value;
}

void initWiFi() {
  delay(1000);
  Serial.println("Connecting to Wi-Fi");

  for (int i = 0; i < NUM_NETWORKS; i++) {
    Serial.println("");
    Serial.print("SSID: ");
    Serial.println(ssids[i]);
    Serial.print("Senha: ");
    Serial.println(passwords[i]);
      
    unsigned long start = millis(); // get the start time
    WiFi.begin(ssids[i], passwords[i]);

    while (WiFi.status() != WL_CONNECTED && millis() - start < 50000){
      Serial.print(".");
      blinkLed(BLUE_LED, 4, 50, 50); // equivalent to delay(300)
    }

    if (WiFi.status() == WL_CONNECTED) break;
  }

  

  if (WiFi.status() != WL_CONNECTED) {
    blinkLed(RED_LED, 3, 50, 50, true);
    return;
  } else {
    blinkLed(GREEN_LED, 0, 50, 50, true);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}


void connectFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Serial.print("Connecting to Firebase ... ");

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.print("Connected");
    blinkLed(GREEN_LED, 5, 50, 50, true);
  } else{
    blinkLed(RED_LED);
    Serial.printf("%s\n", config.signer.signupError.message.c_str()); // Mensagem de erro de coneção ao Firebase.
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
