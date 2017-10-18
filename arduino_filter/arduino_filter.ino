/* N: Sample length ( x[n - N + 1] to x[n] ) (inclusive) which means the order is N - 1 */
# define N 6

int GAIN; // scaling factor (in the code values are DIVIDED by GAIN so as to avoid using float values for scaling)
int DC;
int x[N] = {0}; // buffer to save N values: x[n - N + 1] to x[n] (inclusive)

// filter coeffecients for the FIR filter, only first N are used

//int filter_coeffs[100] = {-3, -25, 155, -25, -3}; // 0.25, HIGH
int filter_coeffs[100] = {3, 25, 71, 71, 25, 3}; // 0.25, LOW
//int filter_coeffs[100] = {-2, -10, -34, 149, -34, -10, -2}; // 0.25, HIGH
//int filter_coeffs[100] = {-8, -40, 93, 93, -40, -8}; // 0.45 -> 0.55, PASS
//int filter_coeffs[100] = {-4, -47, 90, 90, -47, -4}; // 0.4 -> 0.7, PASS
//int filter_coeffs[100] = {1, -6, -47, 120, -47, -6, 1}; // 0.4, HIGH
//int filter_coeffs[100] = {0, 19, 80, 80, 19, 0}; // 0 -> 0.4, LOW
//int filter_coeffs[100] = {1, 1, 1, 1, 1, 1, 1};

int p = 0; // index current position being addressed in buffer
unsigned char ch; // unsigned char to store the incoming byte

void calculate_gain_dc()
{
  /* Calculate appropriate value of gain so that audio remains in 0-255 range */
  GAIN = 0;
  DC = 0;
  for(int i = 0; i < N; i++){ 
    GAIN += abs(filter_coeffs[i]);
    DC += -min(filter_coeffs[i], 0);
  
  }
    
  DC *= 255; // DC before division by GAIN
}

unsigned int filter(){
  /* Simple FIR filter.
   * Works by summing and scaling the last N values of input
   * The scaling has to be chosen so as to ensure the response will be in 8bit range
   * 8bit range: 0 - 255
   */
  long int y = 0; // long to avoid overflow
  int x_index;

  for(int i = 0; i < N; i++){
    
    x_index = p - 1 - i;
    if (x_index < 0) x_index = N + x_index;
    
    y += filter_coeffs[i]*x[x_index];
    
  }
  
  return ((unsigned int) ((y + DC)/GAIN)); // DC offset so as to not go below 0
  /* divide by GAIN only at the end to reduce execution time
   * however this can cause overflow of y; GAIN < 256 to ensure y doesn't overflow
   */
}

void setup() {

  calculate_gain_dc();
  Serial.begin(250000); // a very high bitrate is required to transfer audio in real time
  Serial.println("Ready!"); // let the computer know that the Arduino is set up
}

void loop() {
  if(Serial.available()){ // only filter if input is present
    
    Serial.readBytes(&ch, 1); // read 1 byte
    x[p] = ((unsigned int) ch); // implicit type casting will happen to unsigned int
    
    p++; // go to next index
    if(p == N) p = 0; // wrap around if index == N
   
    Serial.write((unsigned char) filter()); // return processed byte
  }
}
