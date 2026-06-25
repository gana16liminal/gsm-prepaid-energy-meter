

#include <EEPROM.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int led = 13;
#define pulsein 8
#define relay 12

unsigned int pulse_count = 0;
float units = 0;
unsigned int rupees = 0;
float watt_factor = 0.3125;
unsigned int temp = 0, i = 0, x = 0, k = 0;
char str[70];
char flag1 = 0, flag2 = 0;
String bal = "";

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(led, OUTPUT);
  pinMode(pulsein, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(pulsein, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("Automatic Energy");
  lcd.setCursor(0, 1);
  lcd.print(" Meter ");
  delay(2000);

  lcd.clear();
  lcd.print("Circuit Digest");
  delay(2000);

  lcd.clear();
  lcd.print("GSM Initilizing...");
  gsm_init();

  lcd.clear();
  lcd.print("System Ready");

  Serial.println("AT+CNMI=2,2,0,0,0");
  init_sms();
  send_data("System Ready");
  send_sms();
  delay(1000);

  digitalWrite(led, LOW);
  lcd.clear();
}

void loop() {
  serialEvent();

  rupees = EEPROM.read(1);
  units = rupees / 5.0;

  lcd.setCursor(0, 0);
  lcd.print("Units:");
  lcd.print(units);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  if (rupees < 15)
    lcd.print("LOW Balance:");
  else
    lcd.print("Balance:");
  lcd.print(rupees);
  lcd.print(" ");

  read_pulse();
  check_status();

  if (temp == 1) {
    decode_message();
    send_confirmation_sms();
  }
}

// Reads incoming serial data from the GSM module / recharge SMS
void serialEvent() {
  while (Serial.available()) {
    char ch = (char)Serial.read();
    str[i++] = ch;
    if (ch == '*') {
      temp = 1;
      lcd.clear();
      lcd.print("Message Received");
      delay(500);
      break;
    }
  }
}

// Puts GSM module into SMS text mode and sets recipient number
void init_sms() {
  Serial.println("AT+CMGF=1");
  delay(200);
  Serial.println("AT+CMGS=\"+918287114222\""); // replace with registered number
  delay(200);
}

void send_data(String message) {
  Serial.println(message);
  delay(200);
}

// Ctrl+Z (ASCII 26) terminates and sends the SMS
void send_sms() {
  Serial.write(26);
}

// Reads pulses from the energy meter via optocoupler, decrements balance
void read_pulse() {
  if (!digitalRead(pulsein)) {
    digitalWrite(led, HIGH);
    if (units < 1) {
      // no units left, do nothing further here
    } else {
      units--;
    }
    rupees = units * 5;
    EEPROM.write(1, rupees);

    while (!digitalRead(pulsein)); // wait for pulse to clear
    digitalWrite(led, LOW);
  }
}

// Checks balance thresholds and triggers relay/SMS actions
void check_status() {
  if (rupees > 15) {
    digitalWrite(relay, HIGH);
    flag1 = 0;
    flag2 = 0;
  }

  if (rupees < 15 && flag1 == 0) {
    lcd.setCursor(0, 1);
    lcd.print("LOW Balance ");

    init_sms();
    send_data("Energy Meter Balance Alert:");
    send_data("Low Balance\n");
    Serial.println(rupees);
    delay(200);
    send_data("Please recharge your energy meter soon.\n Thank you");
    send_sms();
    message_sent();

    flag1 = 1;
  }

  if (rupees < 5 && flag2 == 0) {
    digitalWrite(relay, LOW);

    lcd.clear();
    lcd.print("Light Cut Due to");
    lcd.setCursor(0, 1);
    lcd.print("Low Balance");
    delay(2000);

    lcd.clear();
    lcd.print("Please Recharge ");
    lcd.setCursor(0, 1);
    lcd.print("UR Energy Meter ");

    init_sms();
    send_data("Energy Meter Balance Alert:\nLight cut due to low Balance\nPlease recharge your energy meter soon.\n Thank you");
    send_sms();
    message_sent();

    flag2 = 1;
  }
}

// Extracts recharge amount sent between '#' and '*' in the SMS
void decode_message() {
  x = 0; k = 0; temp = 0;
  while (x < i) {
    while (str[x] == '#') {
      x++;
      bal = "";
      while (str[x] != '*') {
        bal += str[x++];
      }
    }
    x++;
  }
  bal += '\0';
}

// Applies the recharge amount and sends confirmation SMS
void send_confirmation_sms() {
  int recharge_amount = bal.toInt();
  rupees += recharge_amount;
  EEPROM.write(1, rupees);

  lcd.clear();
  lcd.print("Energy Meter ");
  lcd.setCursor(0, 1);
  lcd.print("Recharged:");
  lcd.print(recharge_amount);

  init_sms();
  send_data("Energy Meter Balance Alert:\nYour energy meter has been recharged Rs:");
  send_data(bal);
  send_data("Total Balance:");
  Serial.println(rupees);
  delay(200);
  send_data("Electricity Has Been Connected\nThank you");
  send_sms();

  temp = 0; i = 0; x = 0; k = 0;
  delay(1000);
  message_sent();
}

void message_sent() {
  lcd.clear();
  lcd.print("Message Sent.");
  delay(1000);
}

// Initializes the GSM module: checks connection, disables echo, waits for network
void gsm_init() {
  lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag = 1;
  while (at_flag) {
    Serial.println("AT");
    while (Serial.available() > 0) {
      if (Serial.find("OK"))
        at_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);

  lcd.clear();
  lcd.print("Disabling ECHO");
  boolean echo_flag = 1;
  while (echo_flag) {
    Serial.println("ATE0");
    while (Serial.available() > 0) {
      if (Serial.find("OK"))
        echo_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);

  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag = 1;
  while (net_flag) {
    Serial.println("AT+CPIN?");
    while (Serial.available() > 0) {
      if (Serial.find("+CPIN: READY"))
        net_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();
}

