#define next "/"
#define wait ":"
#define finish "\0"

#define motor1on "1,"
#define motor2on "2,"
#define motor3on "3,"
#define motor4on "4,"
#define motor5on "5,"
#define motor6on "6,"
#define motor7on "7,"
#define soundon "8,"

#define motor1off "-1,"
#define motor2off "-2,"
#define motor3off "-3,"
#define motor4off "-4,"
#define motor5off "-5,"
#define motor6off "-6,"
#define motor7off "-7,"
#define soundoff "-8,"

#include <avr/pgmspace.h>

/*
  rindas formats:
  "laiks" wait ko ieslegt un izslegt next
  "laiks" noraadam laiku sekundees
  "laiks" obligaati jaaieliek pedinaas
  peec laika obligaati vajag vaardu wait
  katra darbiiba (soundon,soundoffmotor1on,motor1off utt) jaatdala ar atstarpi
  visur lietot tikai mazos burtus
  rindas beigaas obligaati vajag next
  programmas beigaas obligaati vajag vaardu finish

  piemeers:
  "0" wait soundon motor1on motor2on next
  "10" wait motor1off next
  "10" wait motor2off next
  "20" wait sounoOff next
  finish
*/

const String boot PROGMEM =

  // Write your program here
  "0" wait soundoff motor1on motor2on next
  finish
  // end of program

  ;

const String standby PROGMEM =

  // Write your program here
  "0" wait next
  finish
  // end of program

  ;

  
const String program PROGMEM =


  // Write your program here
  "0" wait soundon motor6off motor7off motor3on motor4on next
  "10" wait motor3off next
  "10" wait motor4off next
  "20" wait soundoff next
  finish
  // end of program

  ;

#define inputPin 13
#define inputMode INPUT_PULLUP
//#define invert

#define pin0 2
#define pin1 3
#define pin2 4
#define pin3 5
#define pin4 6
#define pin5 7
#define pin6 8
#define pin7 9

const uint8_t pins[] PROGMEM = {pin0,pin1,pin2,pin3,pin4,pin5,pin6,pin7};

typedef struct _execData {
  long del;
  uint8_t pins;
};

_execData execData;

void listPinState() {
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("Bit:\t");
    Serial.print(i+1);
    Serial.print(":\t");
    if (bitRead(execData.pins, i)) {
      Serial.println("On");
    }
    else {
      Serial.println("Off");
    }
  }
}

void digitToPin(String s) {
  String tmp;
  int i,j=0, jOld = 0;
  while ( j < s.length() ) {
    j = s.indexOf(",", jOld);
    if (j < 0) break;
    tmp = s.substring(jOld, j);
    i = tmp.toInt();
    if (i > 0) bitSet(execData.pins, i - 1);
    else bitClear(execData.pins, abs(i)-1);
    jOld = j + 1;
  }
}

void setPins(){
  for (uint8_t i=0;i<8;i++){
    if (bitRead(execData.pins, i)) {
      digitalWrite(pins[i],HIGH);
    }
    else {
      digitalWrite(pins[i],LOW);
    }
  }
  delay(execData.del);
}
void parseData(String s) {
  String tmp;
  int i;
  i = s.indexOf(wait);
  //Serial.println(s);
  tmp = s.substring(0, i);
  //Serial.println(tmp);
  execData.del = tmp.toInt() * 1000;
  //Serial.print("Delay (ms): ");
  //Serial.println(execData.del);
  //Serial.println("Old state: ");
  //listPinState();
  tmp=s.substring(i+1);
  digitToPin(tmp);
  setPins();
  //Serial.println("New state: ");
  //listPinState();
}
void parseLines(String s) {
  int j=0, jOld = 0;
  while ( j < s.length() ) {
    j = s.indexOf(next, jOld);
    if (j < 0) break;
    parseData(s.substring(jOld, j));
    jOld = j + 1;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(inputPin,inputMode);
  for (uint8_t i=0;i<8;i++){
    pinMode(pins[i],OUTPUT); 
  }
  
  Serial.println("Exec BOOT");
  parseLines(boot);

}

bool inputTrigger(){
  #ifdef invert
    return digitalRead(inputPin);
  #endif
  #ifndef invert
    return !digitalRead(inputPin);
  #endif
}
void loop() {
  if (inputTrigger()){
    Serial.println("Exec PROGRAM");
    parseLines(program);
  } else {
    Serial.println("Exec STANDBY");
    parseLines(standby);
  }
}
