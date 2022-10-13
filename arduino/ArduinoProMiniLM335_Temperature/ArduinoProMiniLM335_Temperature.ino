// LM335 Temperature module for Arduino Pro Mini
// Copyright (c) 2022 by David Van Wagner
// davevw.com
// Open source: MIT License

void setup() {
  Serial.begin(115200);
}

void readTemp(int& raw, int& fahrenheit)
{
  raw = analogRead(A6);
  
  // calibrated linear formula from two measured data points, depends on exact running voltage
  // raw=564, fahrenheit=37
  // raw=605, fahrenheit=65
  fahrenheit = ((raw-510)*28)/41;
}

void readAverageTemp(int& raw, int& fahrenheit, int count)
{
  long raw_total = 0;
  long fahrenheit_total = 0;

  for (int i=0; i<count; ++i)
  {
   readTemp(raw, fahrenheit);
   raw_total += raw;
   fahrenheit_total += fahrenheit;
  }

  raw = raw_total / count;
  fahrenheit = fahrenheit_total / count;
}

void loop() 
{  
  int raw, fahrenheit;

  readAverageTemp(raw, fahrenheit, 50);
  Serial.print("raw="); Serial.println(raw);
  Serial.print("fahrenheit="); Serial.println(fahrenheit);

  delay(1000);
}
