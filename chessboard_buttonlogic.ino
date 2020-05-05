// Code Programmer: Christian Escutia
// Chessboard game logic, takes input from pins which are in binary from 1,2,4,8,16,32,64 to then send to the Raspberry PI 3
// once data sent the Raspberry Pi handles the Game Logic, arduino is just a relay and counter.
// Can techically use to make even bigger board games past 64 squares but power requirements would grow as will logic games.

// Initialses the Pins for Data Collection
int pin1 = 1; //binary 1
int pin2 = 2; //binary 2
int pin3 = 3; //binary 4
int pin4 = 4; //binary 8
int pin5 = 5; //binary 16
int pin6 = 6; //binary 32
int pin7 = 7; //binary 64
int pin8 = 8; //undo move pin - not implemented
int OutputPin = 13;  // serial connection between arduino and Raspberry PI




//This makes the Connection to the Pi from the Arduino
void setup() {
  Serial.begin(9600);
  pinMode(OutputPin, OUTPUT);
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(pin6, INPUT);
  pinMode(pin7, INPUT);
  pinMode(pin8, INPUT);
  delay(1000);// waits for a second To Let Raspberry Pi Catchup just in case
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  if( digitalRead(pin1) || digitalRead(pin2) || digitalRead(pin3) || digitalRead(pin4) || digitalRead(pin5) || digitalRead(pin6)){
    //checks for a high state from the 6 pins the 7 is only for the last pin so ignored
    delay(10);
    int square = 0;
    if(digitalRead(pin1)){
      sqaure = square + 1;
    }
    if(digitalRead(pin2)){
      sqaure = square + 2;
    }
    if(digitalRead(pin3)){
      sqaure = square + 4;
    }
    if(digitalRead(pin4)){
      sqaure = square + 8;
    }
    if(digitalRead(pin5)){
      sqaure = square + 16;
    }
    if(digitalRead(pin6)){
      sqaure = square + 32;
    }
    digitalWrite(OutputPin, square); 
  }

  //check for last pin which is square 64
  if(digitalRead(pin7)){
    //square 64
    int square = 64;
    digitalWrite(OutputPin, square);
    
  }

  //checks for undo last move pin
  if(digitalRead(pin8)){
    // set undo move to square 0 since there is no square 0 and easier to send data that way and smaller
    int square = 0;
    digitalWrite(OutputPin, square);
  }

}
