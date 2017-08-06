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
  "1" wait soundon motor1on motor2on next
  "10" wait motor1off next
  "10" wait motor2off next
  "20" wait sounoOff next
  finish
*/

const String boot PROGMEM =

  // Write your program here
  "3" wait motor1on motor2on next
  "1" wait motor1off motor2off next
  finish
  // end of program

  ;

const String standby PROGMEM =
  // Write your program here
  "1" wait motor1off motor2off motor3off motor4off motor5off motor6off motor7off soundoff next
  finish
  // end of program

  ;
  
const String program PROGMEM =
  // Write your program here
  "6" wait motor3on motor4on motor5on next

  "5" wait motor2on motor3off motor6on motor7on next

  "6" wait motor3on soundon next
  "2" wait motor1on motor4off motor5off motor7off next
  finish
  // end of program

  ;

/*
#define pinIn0 A0
#define pinIn1 A1
#define pinIn2 A2
#define pinIn3 A3
#define pinIn4 A4
#define pinIn5 A5
*/
#define inputPin A5
//#define inputMode INPUT_PULLUP
#define inputMode INPUT
#define invert

#define pin0 4
#define pin1 5
#define pin2 6
#define pin3 7
#define pin4 8
#define pin5 9
#define pin6 10
#define pin7 11
#define pinLatch 12

//const uint8_t inputPins[] = {pinIn0,pinIn1,pinIn2,pinIn3,pinIn4,pinIn5};

const uint8_t pins[] = {pin0,pin1,pin2,pin3,pin4,pin5,pin6,pin7};

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
  digitalWrite(pinLatch,HIGH);
  for (uint8_t i=0;i<8;i++){
      //Serial.print("Pin: ");
      //Serial.print(i);

    if (bitRead(execData.pins, i)) {
      //Serial.println(" HIGH");
      digitalWrite(pins[i],HIGH);
    }
    else {
      //Serial.println(" LOW");
      digitalWrite(pins[i],LOW);
    }
  }  
  digitalWrite(pinLatch,LOW);
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
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  /*
  for (uint8_t i=0;i<6;i++){
    pinMode(inputPins[i],inputMode);
  }
  */
  pinMode(inputPin,inputMode);
  for (uint8_t i=0;i<8;i++){
    pinMode(pins[i],OUTPUT);
    digitalWrite(pins[i],LOW); 
  }

  pinMode(pinLatch,OUTPUT);
  
  Serial.println("Exec BOOT");
  parseLines(boot);

}


bool inputTrigger(){
      #ifdef invert
        if (!digitalRead(inputPin))
          return true;
      #endif      
      #ifndef invert
        if (digitalRead(inputPin))
          return true;
      #endif      
 
  /*
    for (uint8_t i=0;i<6;i++){
        Serial.print("Pin: ");
        Serial.print(i);
        Serial.print(", state: ");
        Serial.println(digitalRead(inputPins[i]),BIN);
      #ifdef invert
        if (!digitalRead(inputPins[i]))
          return true;
      #endif      
      #ifndef invert
        if (digitalRead(inputPins[i]))
          return true;
      #endif      
        
    }
    */
    return false;
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
