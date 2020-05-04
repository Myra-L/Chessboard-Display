
//Row SRs are assumed to be wired together where possible. EG: Scan rate=4, size=32, We will only have 2 SR signal pins, as every second SR will be wired together.
const int row_data_pins[2] = {22, 23};
const int row_clk = 26;
const int row_reset = 36; //Row gets its own reset pin, because its reset button is actually useful.

//Column-data pins are organized by set of SRs, then by # within that set.
//EX: a 1:8 scan rate 64x64 matrix would have 8 sets of column SRs, as it has 8 lines on at any given time.
const int col_data_pins[1][2] = {
  {27, 28}
};
const int col_clk = 35; //all columns use the same clock. They do NOT use the same clock as the rows, as doing that would be catastrophic during setup.
const int col_reset = -1; //columns shou  ld remain *high* when off, and SR's reset brings them to low. So connect their reset pin to Vdd

void setup() {
  // put your setup code here, to run once:
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(row_reset, LOW);
  for (int i = 5; i < 52; i++) {
    //set all GPIO pins to be output. Just for now; I'll figure out which ones need to be input later c:
    pinMode(i, OUTPUT);
  }
  
  for (int i = 0; i < 8; i++) {
    digitalWrite(col_data_pins[0][0], HIGH);
    digitalWrite(col_data_pins[0][1], HIGH);
    digitalWrite(col_clk, LOW);
    delayMicroseconds(5);
    digitalWrite(col_clk, HIGH);
    delayMicroseconds(5);
  }

  digitalWrite(row_reset, HIGH);
  for (int i = 0; i < 10; i++) {
    digitalWrite(row_data_pins[0], HIGH);
    digitalWrite(row_data_pins[1], HIGH);
    digitalWrite(row_clk, LOW);
    delayMicroseconds(5);
    digitalWrite(row_clk, HIGH);
    delayMicroseconds(5);
    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
