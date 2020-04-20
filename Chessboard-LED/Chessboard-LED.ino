#include <timer.h>

/*
 * Define an array of pixel_amt / 8.0 round_up arrays, each being 8. That way we can just map stuff into them, solves our problem of using shift registers pretty easy. 
 * Modular in size, NOT in technology: using 8-bit shift-registers of the same type I used is required. This means many items must be multiples of 8. 
 * Plan: every 25 milliseconds, call a function. That function calls a bunch of smaller functions, each of which enable the LEDs for a single row (or set of such). 
 * 2nd-layer function is on a timer to call its new function every 25ms / # of rows in set. 
 * 3rd-layer function just gets all of its stuff arranged then returns. That way we don't have to deal with blocking delays or anything. 
 * The problem then, is making sure that everything both enters on time and exits on time. 
 * 
 */
 
#define NUM_SRS 1
#define SCAN_RATE 4 //This is the number of rows that will be on at any given point. So, SCAN_RATE = 4, and SIZE = 64, would be equivalent to a 1:16 scan rate.
#define SIZE 32 //Number of LEDs in each row and column.
#define FREQUENCY 40 //Frequency of the display, in Hz
#define PERIOD 1 / FREQUENCY
#define ROW_PERIOD 1 / ((SIZE / SCAN_RATE) * FREQUENCY) //Time each set of rows has to do its thing. 

//Row SRs are assumed to be wired together where possible. EG: Scan rate=4, size=32, We will only have 2 SR signal pins, as every second SR will be wired together.
const int row_data_pins[SIZE / SCAN_RATE] = {22, 23, 24, 25};
const int row_clk = 26;
const int row_reset = 36; //Row gets its own reset pin, because its reset button is actually useful. 

//Column-data pins are organized by set of SRs, then by # within that set.
//EX: a 1:8 scan rate 64x64 matrix would have 8 sets of column SRs, as it has 8 lines on at any given time. 
const int col_data_pins[SIZE / SCAN_RATE][SIZE / 8] = {
  {27, 28, 29, 30},
  {31, 32, 33, 34}
};
const int col_clk = 35; //all columns use the same clock. They do NOT use the same clock as the rows, as doing that would be catastrophic during setup. 
const int col_reset = null; //columns should remain *high* when off, which the SR doesn't support. So connect their reset pin to Vdd

void fullCycle(void *) {
  //DO SOMETHING TO RECEIVE THE INPUT FROM THE PI 
  //DO THAT RIGHT HERE
  //WITH SERIAL INPUT OR SOMETHING

  
}

void setup() {
  Serial.begin(57600)
  digitalWrite(LED_BUILTIN, LOW)
  for(int i = 5; i < 52; i++) {
    //set all GPIO pins to be output. Far as I can tell, they all will be!
    pinMode(i, OUTPUT)
  }

  Timer<5, micros> 1st_timer; 
  1st_timer.every(1000 * PERIOD, fullCycle); 
}

void loop() {
  1st_timer.tick()
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
 * digitalWrite takes about 2us. Every clock cycle of the SR takes about 5-10 us, depending on how much I feel like using non-round increments. 
 * This means 1 write to an SR takes about 12 us (3 writes (2 clock, 1 data) * 2us, 2*3us of delays)
 * A full write-through of the SR (8 total writes to it) takes ~100us. This is, VERY ROUGHLY, 1/15 of the time that each row gets (if 1:16, 64x64). 
 * Not really sure what should be taken from all that. Less than 1/15 of the time should be enough to reduce any unintentional lighting to unnoticable levels.
 * Which means that 
 * /
