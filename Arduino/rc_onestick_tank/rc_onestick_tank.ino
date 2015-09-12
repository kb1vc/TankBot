
int lctrl_mag; // Here's where we'll keep our channel values
int rctrl_mag;

#define CW 0
#define CCW 1

/* matt reilly and Ben Reilly 
 * to running our ardumoto bots
 *
 * one stick control for an ardumoto.  
 * Right stick controls activate pin 6 (up/down)
 * and pin 5 (right left). 
 *
 * Controls work like this: 
 * 
 *  Speed is proportional to stick's distance from the center. 
 *
 *  Direction is related to the angle the stick makes with a vertical
 *  line pointing away from the operator.  
 *
 *  Clockwise angles are +, counterclockwise angles are -
 *
 *
 *       angle      direction
 *         0            forward
 *      +/-180          backward
 *       0..+80         forward turn right
 *      85..95         dead stop
 *     -80..0           forward turn left
 *     100..180         backup to right
 *    -100..180         backup to left
 *     -85..-95        dead stop
 * 
 *  Output pulse <deviation from 1500 uSec> for left and right wheel is determined by
 *  two parts -- speed and ratio of V and H channels. 
 *
 *  Call the right pulse width R and the left pulse width L. 
 * 
 *  For the upper right quadrant, the ratio of the deviation of the R and L
 *  channels follows a linear scheme
 * 
 *  V is the difference between the V channel reading and 1500
 *  H is the difference between the H channel reading and 1500
 
 *  V >= 0, H >= 0
 *  dR / dL  =   1 for H = 0, V any value
 *               -1 for V/H = 5
 * 
 *  for V/H > 5 dR = dL = 0
 *
 *  s = (V^2 + H^2) / (2 * MAX^2)
 *  
 */

const byte H_CHAN = 5;
const byte V_CHAN = 6;

// motor pins for ardumoto shield
const byte PWMA = 3;
const byte PWMB = 11;
const byte DIRA = 12;
const byte DIRB = 13; 

#define LMOTOR 0
#define RMOTOR 1

void setup() {

  pinMode(5, INPUT); // Set our input pins as such
  pinMode(6, INPUT);
 
  setupArdumoto();
  Serial.begin(9600);
}

void loop() {

  
  int v_mag = pulseIn(V_CHAN, HIGH, 25000); // Read the pulse width of 
  int h_mag = pulseIn(H_CHAN, HIGH, 25000); // each channel

  float v = ((v_mag - 1960) >> 3); 
  float h = ((h_mag - 1960) >> 3); 
  if(fabs(v) < 2.0) v = 0.0; 
  if(fabs(h) < 2.0) h = 0.0; 
  
  float spd = 0.1 * (v * v + h * h);
  
  if(spd < 10) spd = 0.0; 
  if(fabs(h) > (5.0 * fabs(v))) spd = 0.0; 
  
  float l, r; 
  
  // there is no radio attached...
  if (h < -200) spd = 0.0; 
  if (v < -200) spd = 0.0; 
  
  float sspd = sqrt(spd) + 0.001; 
  
  // first assume we are in the upper right quadrant. 
  l = 1.0; 
  r = 1.0 - 0.4 * fabs(h / sspd); 
  if(r < -1.0) r = -1.0; 
  
  if(v > 0) {
    // moving forward
    if(h > 0) {
      // upper right
    }
    else {
      // upper left
      float tmp = l; 
      l = r; 
      r = tmp; 
    }
  }
  else {
    if(h > 0) {
      // lower right
      l = -l; 
      r = -r; 
    }
    else {
      // lower left
      float tmp = -l;
      l = -r; 
      r = tmp;
    }
  }
  Serial.print("H: "); Serial.print(h); Serial.print(" V: "); Serial.print(v);
  Serial.print("Channel L: ");
  Serial.print(l);
  
  Serial.print(" Channel R: ");
  Serial.println(r);
  
  Serial.print(" speed: ");
  Serial.println(spd);
  
    
  l = l * spd; 
  r = r * spd; 
  
  if(spd > 0.0) {
    int is = ((int)(fabs(l))); 
    byte s = (is > 255) ? 255 : ((byte) is);
    Serial.print("L is = "); Serial.print(is); Serial.print(" s = "); Serial.println(s); 
    if(l < -10) {
      driveArdumoto(LMOTOR, CCW, s); 
    }
    else if(l > 10) {
      driveArdumoto(LMOTOR, CW, s);
    }
    else {
      stopArdumoto(LMOTOR);
    }
    
    is = ((int) (fabs(r))); 
    s = (is > 255) ? 255 : ((byte) is);
    Serial.print("R is = "); Serial.print(is); Serial.print(" s = "); Serial.println(s); 
    
    if (r < -10) {
      driveArdumoto(RMOTOR, CCW, s);
    }
    else if(r > 10) {
      driveArdumoto(RMOTOR, CW, s);
    }
    else {
      stopArdumoto(RMOTOR);
    }
  }
  else {
      stopArdumoto(LMOTOR);
      stopArdumoto(RMOTOR);
  }

  delay(100); // I put this here just to make the terminal 
              // window happier
}

void driveArdumoto(byte motor, byte dir, byte spd)
{ 
    if(motor == LMOTOR) {
      digitalWrite(DIRA, dir);
      analogWrite(PWMA, spd); 
    }
    else if(motor == RMOTOR) {
      digitalWrite(DIRB, dir);
      analogWrite(PWMB, spd); 
    }
}

void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}


// setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}

