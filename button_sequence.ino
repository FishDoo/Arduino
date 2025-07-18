void updateLeds(int state);
void handleSerial();
void printUsage();
int parsePin(String token);
bool parseValue(String token, int &val);

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

int parsePin(String token) {
  token.trim();
  token.toUpperCase();
  if (token.length() < 2) return -1;
  char type = token.charAt(0);
  int num = token.substring(1).toInt();
  if (type == 'D' && num >= 2 && num <= 9) return num;
  if (type == 'A' && num >= 0 && num <= 5) return A0 + num;
  return -1;
}

bool parseValue(String token, int &val) {
  token.trim();
  token.toUpperCase();
  if (token == "HIGH" || token == "H") {
    val = HIGH;
    return true;
  }
  if (token == "LOW" || token == "L") {
    val = LOW;
    return true;
  }
  return false;
}

void printUsage() {
  Serial.println(F("========== Arduino USB GPIO Control =========="));
  Serial.println(F("Command examples:"));
  Serial.println(F("Set pin HIGH:    SET D3 HIGH  (S D3 H)"));
  Serial.println(F("Set pin LOW:     SET D3 LOW   (S D3 L)"));
  Serial.println(F("Read single pin: READ D3      (R D3)"));
  Serial.println(F("Read all pins:   READ ALL     (R A)"));
  Serial.println(F("Toggle control:  CTL ON/OFF"));
  Serial.println(F("Supported pins:  D2~D9, A0~A5"));
  Serial.println(F("================================================"));
}

void handleSerial() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "CTL ON") {
    serialControl = true;
    Serial.println(F("Serial control enabled"));
    return;
  }
  if (cmd == "CTL OFF") {
    serialControl = false;
    Serial.println(F("Serial control disabled"));
    return;
  }

  if (cmd == "STATUS") {
    cmd = "READ ALL";
  }

  if (cmd.startsWith("SET ") || cmd.startsWith("S ")) {
    if (!serialControl) {
      Serial.println(F("Serial control disabled"));
      return;
    }
    int idx = cmd.startsWith("SET ") ? 4 : 2;
    String rest = cmd.substring(idx);
    rest.trim();
    int space = rest.indexOf(' ');
    if (space > 0) {
      String pinStr = rest.substring(0, space);
      String valStr = rest.substring(space + 1);
      int pin = parsePin(pinStr);
      int val;
      if (pin != -1 && parseValue(valStr, val)) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, val);
        Serial.print(pinStr);
        Serial.print('=');
        Serial.println(val == HIGH ? F("HIGH") : F("LOW"));
      }
    }
    return;
  }

  if (cmd.startsWith("READ ") || cmd.startsWith("R ")) {
    int idx = cmd.startsWith("READ ") ? 5 : 2;
    String rest = cmd.substring(idx);
    rest.trim();
    if (rest == "ALL" || rest == "A") {
      Serial.print("D3=");
      Serial.print(digitalRead(ledPins[0]) ? "HIGH" : "LOW");
      Serial.print(" D4=");
      Serial.print(digitalRead(ledPins[1]) ? "HIGH" : "LOW");
      Serial.print(" D5=");
      Serial.println(digitalRead(ledPins[2]) ? "HIGH" : "LOW");
    } else {
      int pin = parsePin(rest);
      if (pin != -1) {
        Serial.print(rest);
        Serial.print('=');
        Serial.println(digitalRead(pin) ? F("HIGH") : F("LOW"));
      }
    }
  }
}
