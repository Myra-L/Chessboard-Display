#include <timer.h>

/*
   Define an array of pixel_amt / 8.0 round_up arrays, each being 8. That way we can just map stuff into them, solves our problem of using shift registers pretty easy.
   Modular in size, NOT in technology: using 8-bit shift-registers of the same type I used is required. This means many items must be multiples of 8.
   Plan: every 25 milliseconds, call a function. That function calls a bunch of smaller functions, each of which enable the LEDs for a single row (or set of such).
   2nd-layer function is on a timer to call its new function every 25ms / # of rows in set.
   3rd-layer function just gets all of its stuff arranged then returns. That way we don't have to deal with blocking delays or anything.
   The problem then, is making sure that everything both enters on time and exits on time.

*/

#define NUM_SRS 1
#define SCAN_RATE 4 //This is the number of rows that will be on at any given point. So, SCAN_RATE = 4, and SIZE = 64, would be equivalent to a 1:16 scan rate.
#define SIZE 32 //Number of LEDs in each row and column.
#define FREQUENCY 40 //Frequency of the display, in Hz (board-cycle)
#define PERIOD 1 / FREQUENCY //Time for each board-cycle
#define ROW_PERIOD 1 / ((SIZE / SCAN_RATE) * FREQUENCY) //Time for each row-cycle. Or... time each set of SCAN_RATE rows has to do its thing. 

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
const int col_reset = -1; //columns should remain *high* when off, and SR's reset brings them to low. So connect their reset pin to Vdd

void boardCycle(void *) {
  //DO SOMETHING TO RECEIVE THE INPUT FROM THE PI
  //DO THAT RIGHT HERE
  //WITH SERIAL INPUT OR SOMETHING


}

void setup() {
  Serial.begin(57600);
  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 5; i < 52; i++) {
    //set all GPIO pins to be output. Just for now; I'll figure out which ones need to be input later c:
    pinMode(i, OUTPUT);
  }

  Timer<5> first_timer;
  //first_timer.every(PERIOD, fullCycle);
}

void loop() {
 // 1st_timer.tick();
}

//A function that probably won't be used, writes the needed values of a single SR. 
//This probably won't be used, because having a seperate delay cycle for each SR is super inefficient.
//each value[] should be either 0 or 1
void writeSR(int data_pin, int clk_pin, int values[8]) {
  digitalWrite(clk_pin, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(data_pin, values[i]) ;
    digitalWrite(clk_pin, LOW);
    delayMicroseconds(3);
    digitalWrite(clk_pin, HIGH);
    delayMicroseconds(3);
  }
}

/*
 * col_data_pins: one-dimensional int array of SCAN_RATE * SIZE / 8 size. Contains the pins for col SRs, in same order as reading english
 * col_data: one-dimensional int array of SCAN_RATE * SIZE size, contains the data for columns, in same order as reading english. 
 *  This is gonna be a long one, so don't mess up! (These should all be 1s or 0s)
 * col_clk: int, it's. It's the pin that the column SRs use for clk. Duh. 
 * 
 */
 
void lightRow(int col_data_pins[], int col_data[], col_clk) {
  /*
   * I couldn't figure out how to write this in words, so... pseudocode passed off as real code.
   * I don't know exactly how all of this data should be passed in, is the thing. 
   */
   for (int i = 0; i < 8; i++) {
    //write the data for this bit to the column SRs
    for (int j = 0; j < SIZE / 8; j++) {
      digitalWrite(col_data_pins[j], col_data[i + j*8]);
    }
    //Is this the row that's supposed to be on? if yes, write HIGH, if no, write LOW.
    int row_write = (i == (row_num % 8) ? 1 : 0);
    digitalWrite(row_data_pins[row_num / 8], row_write);

    digitalWrite(row_clk, LOW);
    digitalWrite(col_clk, LOW);
    delayMicroseconds(4);
    digitalWrite(row_clk, HIGH);
    digitalWrite(col_clk, HIGH);
    delayMicroseconds(4);
    
   }
}


/*NOTES
   SR requires a time of like .1ns for each cycle. The Arduino can only accurately delay at minimums of 3us, so I'm most likely gonna use increments of 5us.
   Oh, while I'm thinking about it I should probably also put this on github.
   digitalWrite takes about 2us. Every clock cycle of the SR takes about 5-10 us, depending on how much I feel like using non-round increments.
   This means 1 write to an SR takes about 12 us (3 writes (2 clock, 1 data) * 2us, 2*3us of delays)
   A full write-through of the SR (8 total writes to it) takes ~100us. This is, VERY ROUGHLY, 1/15 of the time that each row gets (if 1:16, 64x64).
   Not really sure what should be taken from all that. Less than 1/15 of the time should be enough to reduce any unintentional lighting to unnoticable levels.
   Which means that
   */
