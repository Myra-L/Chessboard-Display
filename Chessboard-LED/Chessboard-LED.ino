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
#define SCAN_RATE 1 //This is the number of rows that will be on at any given point. So, SCAN_RATE = 4, and SIZE = 64, would be equivalent to a 1:16 scan rate.
#define SIZE 16 //Number of LEDs in each row and column.
#define ROW_GROUP_SIZE 16 //number of Rows in a group of rows. IE: SIZE / SCAN_RATE
#define FREQUENCY 40 //Frequency of the display, in Hz (board-cycle)
#define PERIOD (1 / FREQUENCY) //Time for each board-cycle
#define ROW_PERIOD (1 / ((ROW_GROUP_SIZE) * FREQUENCY)) //Time for each row-cycle. Or... time each set of SCAN_RATE rows has to do its thing. 

//Row SRs are assumed to be wired together where possible. EG: Scan rate=4, size=32, We will only have 2 SR signal pins, as every second SR will be wired together.
const int row_data_pins[SIZE / SCAN_RATE] = {22, 23};
const int row_clk = 26;
const int row_reset = 36; //Row gets its own reset pin, because its reset button is actually useful.

//Column-data pins are organized by set of SRs, then by # within that set.
//EX: a 1:8 scan rate 64x64 matrix would have 8 sets of column SRs, as it has 8 lines on at any given time.
const int col_data_pins[SIZE / ROW_GROUP_SIZE][SIZE / 8] = {
  {27, 28}
};
const int col_clk = 35; //all columns use the same clock. They do NOT use the same clock as the rows, as doing that would be catastrophic during setup.
const int col_reset = -1; //columns shou  ld remain *high* when off, and SR's reset brings them to low. So connect their reset pin to Vdd

//used for testing, hence the name
/*int test_serial[SIZE * SIZE] = {
  1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
};*/

int test_serial[SIZE * SIZE] = {
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 
  0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1,
};

Timer<5, micros> over_timer;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function definition

//advances the SR with that clock pin by one cycle. 
//Cuz GOD typing these four lines over and over got old. SHoulda done this sooner. 
void advanceClock(int pin) {
  digitalWrite(pin, LOW);
  delayMicroseconds(3);
  digitalWrite(pin, HIGH);
  delayMicroseconds(3);
}

//A function that probably won't be used, writes the needed values of a single SR. 
//This probably won't be used, because having a seperate delay cycle for each SR is super inefficient.
//each value[] should be either 0 or 1
void writeSR(int data_pin, int clk_pin, int values[8]) {
  digitalWrite(clk_pin, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(data_pin, values[i]) ;
    advanceClock(clk_pin);
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

  rowArgs(int c[SIZE*SIZE] = {}, int r = 0, int i = 0, int count = SIZE * SIZE) : row_num(r), iteration(i), col_data_count(count) {
    col_data = new int[col_data_count];
    memcpy(col_data, c, sizeof(c[0]) * count);
  };
};

bool lightRow(void* arg) {
  /*digitalWrite(row_reset, LOW);
  advanceClock(row_clk);
  digitalWrite(row_reset,HIGH);
  advanceClock(row_clk);*/
  //Serial.write("BEGINNING NEW RUN OF LIGHTROW \n\n");
  
  
  rowArgs* args = (rowArgs*)arg;
  auto row_write = LOW;

  digitalWrite(row_reset, LOW);
  delayMicroseconds(4);
  digitalWrite(row_reset, HIGH);
  delayMicroseconds(4);
  int pin;
  int on;
  for (int k = 7; k >= 0; k--) { //We have 8 bits on each SR, so loop 8 times, sleeping on each iteration
    for (int i = 0; i < SCAN_RATE; i++) { //We'll have to write SCAN_RATE lines each cycle, so loop SCAN_RATE times. 
      for (int j = 0; j < (SIZE / 8); j++) { //Again, SRs are 8 bits. This one's really just for convenience (essentially: loop X times, where X is # of SRs used for each line.)
      
        //If SCAN_RATE = 2, SIZE = 16, then we have to loop through 32 different pixels each cycle. 
        //The inner 2 loops could probably be combined, I think this just seems easier. 
          pin = col_data_pins[i][j];
          on = (args->col_data[i * SIZE * ROW_GROUP_SIZE + SIZE * args->row_num + j * 8 + k] == 1 ? HIGH : LOW);
          digitalWrite(pin, on);
        }
    } 
    advanceClock(col_clk);
  }

  
  for (int i = ROW_GROUP_SIZE-1; i >= 0; i--) {
   if (args->row_num / 8 == i / 8) {
     row_write = (i == (args->row_num) ? HIGH : LOW); //Are we on the row we need to be on? 
     digitalWrite(row_data_pins[args->row_num / 8], row_write); //Integer division rounds down. 0-7=>0, 8-15=>1, etc.
     advanceClock(row_clk);
   } else {
    //if we're not even in the right row SR's range, then just skip to the next iteration
      continue;
    }
    
  }
  
  //commented-out bit would be used if we had a function over this, controlling it. 
  /* if (args->iteration >= SIZE / SCAN_RATE)
    return false;
  else 
    return true;
  }*/
  args->row_num = (args->row_num + 1) % (ROW_GROUP_SIZE); //iterate the row count right here in the row function!! No gods, no masters! (disclaimer I'm not a libertarian)
  return true;
}

void initialRead() {
  Serial.write("initialRead is running\n");
  //run this before we start the timer. This should block everything until we get a serial input from the Leader.
  //After that's received, write some stuff to our rowArgs pointer (which should be a global variable, I think.)
  //Then we can start our timer and let everything else run automatically basically. 
}

bool loopRead(void* arg) {
  Serial.write("loopRead is running\n");
  //this is our serial reader that will be ran every... idk, quarter of a second or something? Depends on how fast I can figure out to make the serial reading.
  //Read stuff from the Leader, put it in our rowArgs struct. 
  //Make sure to include a provision for restarting !
  //... wait, do we really need to? Cuz in that case we kinda just wait for more input from the Leader. This is just a video display, after all! 
  
  /*rowArgs* args = (rowArgs*)arg;
  //iteration isn't being used anyway, so I'm gonna go ahead and use it here to test out my stuff. We'll see how it goes! Yay:)
  args->iteration = (args->iteration + 1);
  for (int i = 0; i < args->col_data_count; i++) {
    args->col_data[i] = (args->col_data[i]+1) % 2;
  }*/

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

  digitalWrite(row_reset, LOW);

    for (int k = 7; k >= 0; k--) {
      for (int i = 0; i < SCAN_RATE; i++) {
        for (int j = 0; j < (SIZE / 8); j++) {
          digitalWrite(col_data_pins[i][j], LOW);
        }
      } 
    advanceClock(col_clk);
  }

  digitalWrite(row_reset, HIGH);
  advanceClock(row_clk);
  for (int i = 0; i < 10; i++) {
    digitalWrite(row_data_pins[0], LOW);
    digitalWrite(row_data_pins[1], LOW);
    advanceClock(row_clk);
    
  }

  struct rowArgs row_args;
  row_args.col_data = test_serial;
  initialRead();
  
  over_timer.every(750, lightRow, (void*) &row_args);
  over_timer.every(500000, loopRead, (void*) &row_args);
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
