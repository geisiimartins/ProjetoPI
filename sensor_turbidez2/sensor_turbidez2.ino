/*
  -    SENSOR DE TURBIDEZ DA ÁGUA COM ARDUINO     -
  =================================================
  === BLOG DA ROBOTICA - www.blogdarobotica.com ===
  =================================================
  Autor: Jonas Souza
  E-mail: contato@blogdarobotica.com
  Facebook: facebook.com/blogdarobotica
  Instagram:@blogdarobotica
  YouTube: youtube.com/user/blogdarobotica
  =================================================
  === CASA DA ROBOTICA - www.casadarobotica.com ===
  =================================================
  Facebook: facebook.com/casadaroboticaoficial
  Instagram:@casadarobotica
  ==================================================
*/

int sensorPin = A0; // Atribui a porta A3 a variavel sensorPin
int leitura; //Variável responsável por guardar o valor da leitura analógica do sensor

void setup() {
  Serial.begin(9600); //Inicializa a comunicação serial
}

void loop() {
  leitura = analogRead(sensorPin); //Realiza a leitura analógica do sensor
  Serial.print("Valor lido: "); //Imprime no monitor serial
  Serial.println(leitura); // manda o valor de leitura para o monitor serial
  delay(1000); //Intervalo de 0,5 segundos entre as leituras
  Serial.print("Estado da água: "); //Imprime no monitor serial

  if (leitura > 700) { //Se o valor de leitura analógica estiver acima de 700

    Serial.println("LIMPA"); //Imprime no monitor serial que a água está limpa
  }
  if ((leitura > 600) && (leitura <= 700)) { //Se o valor de leitura analógica estiver entre 600 e 700

    Serial.println("UM POUCO SUJA"); //Imprime no monitor serial que a água está um pouco suja
  }
  if (leitura < 600) { //Se o valor de leitura analógica estiver abaixo de 600 

    Serial.println("MUITO SUJA"); //Imprime no monitor serial que a água está um pouco suja
  }
  Serial.println();
}
