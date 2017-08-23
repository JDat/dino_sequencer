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
  "3000" wait motor1on motor2on next
  "100" wait motor1off motor2off next

  finish
  // end of program

  ;

const String standby PROGMEM =
  // Write your program here
   "1000" wait motor1off motor2off motor3off motor4off motor5off motor6off motor7off soundoff next
  //"1000" wait motor1off motor2off motor3off motor4off motor5off motor6off motor7off soundoff next
  //"1000" wait motor1on motor2on motor3on motor4on motor5on motor6on motor7on soundon next
  finish
  // end of program

  ;
  
const String program PROGMEM =
  // Write your program here
  "4000" wait motor3on motor5on next                        //00:00:08
  "3000" wait motor3off motor4on motor5on next              //00:00:12
  "2000" wait motor2on motor3on motor4off motor5off soundon next  //00:00:15
  "3000" wait motor1on next                                  //00:00:17
  "3000" wait motor2off motor3off motor4on motor5on next    //00:00:20
  "3000" wait motor3on motor4off next                       //00:00:23
  "3000" wait motor3off motor4on motor5off next             //00:00:26
  "4000" wait motor2on motor3on motor4off motor5on next     //00:00:29
  "4000" wait motor3off motor4on motor5off next             //00:00:33
  "4000" wait motor4off next                                //00:00:37
  "2000" wait motor1off motor2off motor3on motor4on motor5on next               //00:00:41
  "5000" wait motor4off motor5off soundoff next                      //00:00:43
  "2000" wait motor4on soundon next                                 //00:00:48
  "4000" wait motor1on motor2on motor4off motor5on next     //00:00:50
  "4000" wait motor3off motor4on motor5off next             //00:00:54
  "4000" wait motor2off motor3on motor4off motor5on next    //00:00:58
  "8000" wait motor2on motor3off motor5off next             //00:01:02
  "4000" wait motor4on next                                 //00:01:10
  "2000" wait motor1off motor2off motor3on motor4off motor5on next  //00:01:14
  "5000" wait motor3off motor4on motor5off soundoff next    //00:01:16
  "2000" wait motor3on motor4on motor5on soundon next       //00:01:21
  "3000" wait motor1on motor3off motor5off next             //00:01:23
  "3000" wait motor2on motor4off next                       //00:01:26
  "3000" wait motor3on motor5on next                        //00:01:29
  "3000" wait motor2off motor3off motor4on motor5on next    //00:01:32 
  "3000" wait motor2on motor4off motor5off next             //00:01:35
  "3000" wait motor4on motor5on next                        //00:01:38
  "3000" wait motor2off motor3on motor4off next             //00:01:41
  "3000" wait motor2on motor3off motor5on next              //00:01:44
  "3000" wait motor1off motor4on next                       //00:01:47             
  "30000" wait motor2off motor4off motor5off soundoff next   //00:01:49   
  //"1000" wait motor1off motor2off motor3off motor4off motor5off soundoff next   
  
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
  //execData.del = tmp.toInt() * 1000;    //for seconds
  execData.del = tmp.toInt();           //for milliseconds
  //Serial.print("Delay (ms): ");
  //Serial.println(execData.del);
  //Serial.println("Old state: ");
  //listPinState();
  tmp=s.substring(i+1);
  digitToPin(tmp);
  setPins();
  Serial.println("New state: ");
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
  //while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  //}
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
