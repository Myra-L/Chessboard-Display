/*
 * Define an array of pixel_amt / 8.0 round_up arrays, each being 8. That way we can just map shit into them, solves our problem of using shift registers pretty easy. 
 * 
 */
#define NUM_SRS 1
#define SCAN_RATE 8
#define SIZE 8 //Assuming that our display is a square. 
const int row[8] = {1,2,3,4,5,6,7,8};
const int col[8] = {1,2,3,4,5,6,7,8};

const int COLUMN_OUT = 46;
const int ROW_OUT = 38;
const int CLK = 50;

void setup() {
  digitalWrite(LED_BUILTIN, LOW);
  // put your setup code here, to run once:
  pinMode(COLUMN_OUT, OUTPUT); 
  pinMode(CLK, OUTPUT); //CLK
  pinMode(ROW_OUT, OUTPUT); //row inputs
  Serial.begin(9600);

  for(int i = 0; i < 8; i++) {
    digitalWrite(COLUMN_OUT, HIGH);
    digitalWrite(ROW_OUT, LOW);
    digitalWrite(CLK, LOW);
    digitalWrite(LED_BUILTIN, LOW);  
    /*if (i == 5) {
      Serial.println("hinkus");
      digitalWrite(COLUMN_OUT, LOW);
      digitalWrite(ROW_OUT, LOW);
    }*/
    delay(25);
    digitalWrite(CLK,HIGH);
    delay(25);
  }
  //digitalWrite(ROW_OUT,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(COLUMN_OUT, LOW);
  digitalWrite(CLK, LOW);
  delay(50);
  digitalWrite(CLK, HIGH);
  delay(50);
}

void writeSR(int location = 0, int possible_locations=8, bool row=true) {
  int SR_pin;
  int false_val;
  if (row) {
    false_val = LOW;
    SR_pin = ROW_OUT;
  } else {
    false_val = HIGH;
    SR_pin = COLUMN_OUT;
  }

  for (int i = 0; i < possible_locations; i++) {
    if (i == location) 
      digitalWrite(SR_pin, !false_val);
    else 
      digitalWrite(SR_pin, false_val);
    digitalWrite(CLK, LOW);
    delay(1);
    digitalWrite(CLK,HIGH);
    delay(1);
  }
}

/*NOTES
 * SR requires a tiem of like .1ns for each cycle. The Arduino can only accurately delay at minimums of 3us, so I'm most likely gonna use increments of 5us. 
 * Oh, while I'm thinking about it I should probably also put this on github. 
 * 
 * /
 */
 
 */
