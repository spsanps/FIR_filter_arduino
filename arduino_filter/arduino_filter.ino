/* N: Sample length ( x[n - N + 1] to x[n] ) (inclusive) which means the order is N - 1 */
# define N 7

unsigned int GAIN; // scaling factor (in the code values are DIVIDED by GAIN so as to avoid using float values for scaling)
unsigned int x[N] = {0}; // buffer to save N values: x[n - N + 1] to x[n] (inclusive)
int filter_coeffs[100] = {-3, -46, -192, 681, -192, -46, -3, -2, 1, 7, // filter coeffecients for the FIR filter, only first N are used
                          3, 1, 8, 2, 7, 2, 2, 7, 8, 1, 
                          7, 3, 7, 6, 2, 5, 1, 9, 5, 1, 
                          8, 8, 8, 8, 8, 8, 8, 8, 8, 8}; 

void calculate_gain_dc()
{
  /* Calculate appropriate value of gain so that audio remains in 0-255 range */
  GAIN = 0;
  for(int i = 0; i < N; i++) GAIN += abs(filter_coeffs[i]);
  GAIN *= 3; // to be on the safe-side and avoid undeflow or overflow (has to be  >= 2 when centered around 128) 
}

unsigned int filter(){
  /* Simple FIR filter.
   * Works by summing and scaling the last N values of input
   * The scaling has to be chosen so as to ensure the response will be in 8bit range
   * 8bit range: 0 - 255
   */
  long int y = 0;
  
  for(int i = 0; i < N; i++) y += filter_coeffs[i]*x[i];

  return ((y + 128)/GAIN); // 128 is DC offset so as to not go below 0 
  /* divide by GAIN only at the end to reduce execution time
   * however this can cause overflow of y; GAIN < 256 to ensure y doesn't overflow
   */
}

void setup() {
  calculate_gain_dc();
  Serial.begin(250000); // a very high bitrate is required to transfer audio in real time
  Serial.println("Ready!"); // let the computer know that the Arduino is set up
}

int p = 0; // index current position being addressed in buffer
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
