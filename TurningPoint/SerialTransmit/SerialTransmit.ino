int value = 0;
bool myClock = false;

void setup() {
  // put your setup code here, to run once:
  value = 0;
  myClock = false;
  pinMode(A0,OUTPUT);
  pinMode(3,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  value++;
  analogWrite(A0,value);
  Serial.println(value);
  delay(10);
  if (value > 1000)
    value = 0;
}
