int entrada_A0;
float tensao;
 
void setup()  {
  Serial.begin(9600);
}
 
void loop() {
  entrada_A0 = analogRead(A0);
  tensao = (entrada_A0 * 5.0) / 1024.0;
  Serial.println(tensao);
  delay(500);
}
