void updateLeds(int state);
void handleSerial();
void printUsage();

bool serialControl = false;  // true = COM port controls LEDs, button disabled

const int buttonPin = 2;
const int ledPins[] = {3, 4, 5};

int currentState = 0; // 0..3
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  printUsage();
  pinMode(buttonPin, INPUT_PULLUP);  // D2 as input with pull-up
  for (int i = 0; i < 3; ++i) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);   // D3~D5 start LOW
  }
}

void loop() {
  if (!serialControl) {
    bool reading = digitalRead(buttonPin);

    // detect button press (transition from HIGH to LOW)
    if (lastButtonState == HIGH && reading == LOW) {
      // simple debounce delay
      delay(50);
      reading = digitalRead(buttonPin);
      if (reading == LOW) {
        currentState = (currentState + 1) % 4;  // cycle states 0..3
        updateLeds(currentState);
      }
    }

    lastButtonState = reading;
  }

  handleSerial();
}

void updateLeds(int state) {
  switch (state) {
    case 0: // D3 HIGH, others LOW
      digitalWrite(ledPins[0], HIGH);
      digitalWrite(ledPins[1], LOW);
      digitalWrite(ledPins[2], LOW);
      break;
    case 1: // D4 HIGH, others LOW
      digitalWrite(ledPins[0], LOW);
      digitalWrite(ledPins[1], HIGH);
      digitalWrite(ledPins[2], LOW);
      break;
    case 2: // D5 HIGH, others LOW
      digitalWrite(ledPins[0], LOW);
      digitalWrite(ledPins[1], LOW);
      digitalWrite(ledPins[2], HIGH);
      break;
    case 3: // all HIGH
      digitalWrite(ledPins[0], HIGH);
      digitalWrite(ledPins[1], HIGH);
      digitalWrite(ledPins[2], HIGH);
      break;
  }
}

void printUsage() {
  Serial.println(F("Commands:"));
  Serial.println(F("  CONTROL ON  - enable serial control"));
  Serial.println(F("  CONTROL OFF - disable serial control"));
  Serial.println(F("  D3 HIGH/LOW"));
  Serial.println(F("  D4 HIGH/LOW"));
  Serial.println(F("  D5 HIGH/LOW"));
  Serial.println(F("  STATUS - report pin states"));
}

void handleSerial() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "STATUS") {
      Serial.print("D3=");
      Serial.print(digitalRead(ledPins[0]) ? "HIGH" : "LOW");
      Serial.print(" D4=");
      Serial.print(digitalRead(ledPins[1]) ? "HIGH" : "LOW");
      Serial.print(" D5=");
      Serial.println(digitalRead(ledPins[2]) ? "HIGH" : "LOW");
    } else if (cmd == "CONTROL ON") {
      serialControl = true;
      Serial.println(F("Serial control enabled"));
    } else if (cmd == "CONTROL OFF") {
      serialControl = false;
      Serial.println(F("Serial control disabled"));
    } else if (serialControl && cmd.startsWith("D3 ")) {
      String val = cmd.substring(3);
      val.trim();
      if (val == "HIGH") digitalWrite(ledPins[0], HIGH);
      else if (val == "LOW") digitalWrite(ledPins[0], LOW);
    } else if (serialControl && cmd.startsWith("D4 ")) {
      String val = cmd.substring(3);
      val.trim();
      if (val == "HIGH") digitalWrite(ledPins[1], HIGH);
      else if (val == "LOW") digitalWrite(ledPins[1], LOW);
    } else if (serialControl && cmd.startsWith("D5 ")) {
      String val = cmd.substring(3);
      val.trim();
      if (val == "HIGH") digitalWrite(ledPins[2], HIGH);
      else if (val == "LOW") digitalWrite(ledPins[2], LOW);
    }
  }
}
