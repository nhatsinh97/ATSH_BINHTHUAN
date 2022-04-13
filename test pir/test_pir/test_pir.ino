int pir = A0, led = 13;
void setup() {
  pinMode(pir, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int data = analogRead(A0);
  if (digitalRead(A1) == 0) {
    digitalWrite(led, HIGH);
  }else{
    digitalWrite(led, LOW);
  }
  Serial.println(data);delay(500);
}
