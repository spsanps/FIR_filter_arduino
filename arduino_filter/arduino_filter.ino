
/* N: Sample length ( x[n - N + 1] to x[n] ) (inclusive) order: N - 1
 * GAIN: Scaling factor (in the code values are DIVIDED by GAIN so as to avoid using float values for scaling)
 */
 
# define N 10
# define GAIN 10


unsigned int x[N] = {0}; // buffer to save N values: x[n - N + 1] to x[n] (inclusive)
int p = 0; // index current position being addressed in buffer


void setup() {
  Serial.begin(250000); // a very high bitrate is required to transfer audio in real time
  Serial.println("Ready!"); // let the computer know that the Arduino is set up
}



unsigned int filter(){
  /* Simple FIR filter.
   * Works by summing and scaling the last N values of input
   * The scaling has to be chosen so as to ensure the response will be in 8bit range
   * 8bit range: 0 - 255
   */
  unsigned int y = 0;
  
  for(int i = 0; i < N; i++) y += x[i];

  return y/GAIN; 
  /* divide by GAIN only at the end to reduce execution time
   * however this can cause overflow of y; N < 256 to ensure y doesn't overflow
   */
}

unsigned char ch; // unsigned char to store the incoming byte

void loop() {
  if(Serial.available()){ // only filter if input is present
    
    Serial.readBytes(&ch, 1); // read 1 byte
    x[p] = ch; // implicit type casting will happen to unsigned int
    
    p++; // go to next index
    if(p == N) p = 0; // wrap around if index == N

    Serial.write((unsigned char) filter()); // return processed byte
  }
}
