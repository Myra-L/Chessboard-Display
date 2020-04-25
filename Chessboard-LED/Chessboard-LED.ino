#include <timer.h>

/*
  Modular in size, NOT technology: must use the same SRs I'm using (or... close to it, I guess. I'll find the exact model and upload the datasheet later. 
  SO, the plan is to use a pretty simple timer library to call our lightRow every so often, with the serialRead function being called even less often.
  Originally the plan was to have a special time in each loop (or maybe every second, idk) set aside for serial reading
    but this way we can do serial reading while the LEDs would be delaying anyway. It's more efficient. I just hope it works c:
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constant definition 

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

int test_serial[SIZE * SCAN_RATE] = {};

Timer<5, micros> over_timer;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function definition


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
 * The struct used to pass arguments into lightRow. 
 * col_data: one-dimensional int array of SCAN_RATE * SIZE size, contains the data for columns, in same order as reading english (left-right, up-down)
 *  This is gonna be a long one, so don't mess up! (These should all be 1s or 0s)
 * row_num: which row in each set of rows is supposed to be on. So, for SCAN_RATE = 4, size = 32, row_num = 0, rows 1, 9, 17, and 25 would be enabled. (if we started counting rows at 1, anyway)
 * iteration: int, the # of times lightRow has been called in this board-wide cycle. IE: when this reaches 15, we're gonna start over at 1. (Might be able to just use row_num for this?
 */
struct rowArgs {
  int* col_data;
  int row_num;
  int iteration;
  int col_data_count;

  rowArgs(int c[SIZE*SCAN_RATE] = {}, int r = 0, int i = 0, int count = SIZE * SCAN_RATE) : row_num(r), iteration(i), col_data_count(count) {
    col_data = new int[col_data_count];
    memcpy(col_data, c, sizeof(c[0]) * count);
  };
};

bool lightRow(void* arg) {
  /*
  * I couldn't figure out how to write this in words, so... pseudocode passed off as real code.
  */
  //reset the rows, that way we 1:don't have to care what the rows were set to before! 2: Hopefully limit glitchiness caused by not bothering to reset our columns. 
  digitalWrite(row_reset, LOW);
  delayMicroseconds(5);
  digitalWrite(row_reset, HIGH);
  delayMicroseconds(5);
  
  rowArgs* args = (rowArgs*)arg;
  for (int i = 0; i < 8; i++) {
    //write the data for this bit to the column SRs
    int row_write = (i == (args->row_num % 8) ? 1 : 0);
    digitalWrite(row_data_pins[args->row_num / 8], row_write); //if 1-8, write to the first pin. If 9-16, write to second pin. If we were doing SCAN_RATE = 2, SIZE = 32
    
    for (int j = 0; j < SIZE / 8; j++) {
      digitalWrite(col_data_pins[j], args->col_data[i + j*8]);
    }
  }
  //Is this the row that's supposed to be on? if yes, write HIGH, if no, write LOW.
  
  digitalWrite(row_clk, LOW);
  digitalWrite(col_clk, LOW);
  delayMicroseconds(4);
  digitalWrite(row_clk, HIGH);
  digitalWrite(col_clk, HIGH);
  delayMicroseconds(4);
  
  //commented-out bit would be used if we had a function over this, controlling it. 
  /* if (args->iteration >= SIZE / SCAN_RATE)
    return false;
  else 
    return true;
  }*/
  args->row_num = (args->row_num + 1) % (SIZE / SCAN_RATE); //iterate the row count right here in the row function!! No gods, no masters! (disclaimer I'm not a libertarian)
  return true;
}

void initialRead() {
  //run this before we start the timer. This should block everything until we get a serial input from the Leader.
  //After that's received, write some stuff to our rowArgs pointer (which should be a global variable, I think.)
  //Then we can start our timer and let everything else run automatically basically. 
}

bool loopRead(void* arg) {
  //this is our serial reader that will be ran every... idk, quarter of a second or something? Depends on how fast I can figure out to make the serial reading.
  //Read stuff from the Leader, put it in our rowArgs struct. 
  //Make sure to include a provision for restarting !
  //... wait, do we really need to? Cuz in that case we kinda just wait for more input from the Leader. This is just a video display, after all! 
  
  rowArgs* args = (rowArgs*)arg;
  //iteration isn't being used anyway, so I'm gonna go ahead and use it here to test out my stuff. We'll see how it goes! Yay:)
  args->iteration = (args->iteration + 1)
  for (int i = 0; i < args->col_data_count; i++) {
    args->col_data[i] = (args->col_data[i]+1) % 2
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Actual functions that do actions

void setup() {
  Serial.begin(57600);
  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 5; i < 52; i++) {
    //set all GPIO pins to be output. Just for now; I'll figure out which ones need to be input later c:
    pinMode(i, OUTPUT);
  }

  for (int i = 0; i < SIZE; i++) {
   test_serial[i*8 + 0] = (i + 1) % 2;
   test_serial[i*8 + 1] = (i) % 2;
   test_serial[i*8 + 2] = (i + 1) % 2;
   test_serial[i*8 + 3] = (i) % 2;
    test_serial[i*8 + 4] = (i + 1) % 2;
   test_serial[i*8 + 5] = (i) % 2;
   test_serial[i*8 + 6] = (i + 1) % 2;
   test_serial[i*8 + 7] = (i) % 2;
  }
  struct rowArgs row_args;
  initialRead();
  over_timer.every(ROW_PERIOD, lightRow, (void*) &row_args);
  over_timer.every(250000, loopRead, (void*) &row_args);
}

void loop() {
  over_timer.tick();
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
