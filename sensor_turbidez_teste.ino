
#include <ESP8266WiFi.h>       // Biblioteca para controlar o WiFi no ESP8266
#include <FirebaseArduino.h>   // Biblioteca para comunicação com o Firebase

// Credenciais de conexão ao Firebase
#define FIREBASE_HOST "https://sensor-turbidez-default-rtdb.firebaseio.com/agua"
#define FIREBASE_AUTH "9p2nAtkKNSP2KEXwXzuYIflX08oGb8QcDOnuRNP3"

// Credenciais de conexão ao WiFi
#define WIFI_SSID "Maciel"
#define WIFI_PASSWORD "maciel777"


int sensorPin = A0;     // Define a porta analógica A0 como entrada do sensor
int leitura;            // Variável para armazenar a leitura analógica do sensor
String estado;          // Variável para armazenar o estado da água

void setup()
{
    Serial.begin(9600);         // Inicializa a comunicação serial com o computador

    // Conecta ao WiFi utilizando as credenciais definidas anteriormente
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando ao WiFi");
    while (WiFi.status() != WL_CONNECTED) {  // Aguarda a conexão ser estabelecida
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Conectado! Endereço IP: ");
    Serial.println(WiFi.localIP()); // Imprime o endereço IP atribuído ao ESP8266

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Inicializa a conexão com o Firebase
}

void loop()
{
    leitura = analogRead(sensorPin);  // Realiza a leitura analógica do sensor

    Serial.print("Valor lido: ");     // Imprime no monitor serial
    Serial.println(leitura);          // Imprime o valor da leitura analógica

    // Armazena o valor da leitura no Firebase
    Firebase.setFloat("agua/turbidez", leitura);
    Serial.println(Firebase.getString(firebaseData, "/agua/turbidez"));

    // Verifica se houve erro na comunicação com o Firebase
   // if (Firebase.failed()) {
    //    Serial.print("Erro ao enviar dados para o Firebase: ");
    //    Serial.println(Firebase.error());
    //    return;
    //}

    delay(1000);                      // Intervalo de 1 segundo entre as leituras

    // Determina o estado da água com base na leitura analógica
    if (leitura > 700) {              // Água limpa
        Serial.print("Estado da água: LIMPA");
    } else if (leitura > 600) {       // Água um pouco suja
        Serial.print("UM POUCO SUJA");
    } else {                          // Água muito suja
        Serial.print("MUITO SUJA");
    }

    // Armazena o estado da água no Firebase
    Firebase.setString("agua/estado", estado);

    // Verifica se houve erro na comunicação com o Firebase
   // if (Firebase.failed()) {
    //    Serial.print("Erro ao enviar dados para o Firebase: ");
    //    Serial.println(Firebase.error());
     //   return;
    //}
  
}
  

    delay(1000);                      // Intervalo de 1 segundo entre as leituras
}
