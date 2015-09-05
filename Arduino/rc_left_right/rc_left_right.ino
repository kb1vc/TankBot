
/*
 RC PulseIn Serial Read out
 By: Nick Poole
 SparkFun Electronics
 Date: 5
 License: CC-BY SA 3.0 - Creative commons share-alike 3.0
 use this code however you'd like, just keep this license and
 attribute. Let me know if you make hugely, awesome, great changes.
 */
 
int lctrl_mag; // Here's where we'll keep our channel values
int rctrl_mag;

#define CW 0
#define CCW 1

/* matt reilly and Ben Reilly -- adapt Nick's code
/* to running our ardumoto bots
*/

const byte LCTRL_CHAN = 5;
const byte RCTRL_CHAN = 6;

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
}

byte speed(int mag) 
{
  int iret; 
  if(mag < 190) {
    iret = (200 - mag) * 6;
  }
  if(mag > 210) { 
    iret = (mag - 200) * 6;
  }
  
  if(iret > 255) return 0xff; 
  else return (iret & 0xff); 
}

void loop() {

  lctrl_mag = pulseIn(LCTRL_CHAN, HIGH, 25000); // Read the pulse width of 
  rctrl_mag = pulseIn(RCTRL_CHAN, HIGH, 25000); // each channel

  // now run the motors...
  lctrl_mag = lctrl_mag / 10; 
  rctrl_mag = rctrl_mag / 10; 
  
  if(lctrl_mag < 190) {
    driveArdumoto(LMOTOR, CCW, speed(lctrl_mag));
  }
  else if (lctrl_mag > 210) {
    driveArdumoto(LMOTOR, CW, speed(lctrl_mag));
  }
  else {
    stopArdumoto(LMOTOR);
  }
  
  if(rctrl_mag < 190) {
    driveArdumoto(RMOTOR, CCW, speed(rctrl_mag));
  }
  else if(rctrl_mag > 210) {
    driveArdumoto(RMOTOR, CW, speed(rctrl_mag));
  }
  else { 
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

