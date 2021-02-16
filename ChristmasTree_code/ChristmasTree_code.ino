// Pins
int ledsGreenPin = 2;
int ledsRedPin = 3;
int buttonPin = 7;
int speakerPin = 5;

// LEDs state (on/off)
bool ledsGreenState = true;
bool ledsRedState = true;

// Christmas melody "Deck the Halls"
int length = 20;
char notes[] = "DCbagabg abCabagsg "; // a space represents a rest
int beats[] = { 3, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 3, 1, 2, 2, 3, 5 };
int tempo = 150;

void setup() {
  // inputs & outputs
  pinMode(ledsGreenPin, OUTPUT);
  pinMode(ledsRedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(speakerPin, OUTPUT);

  // initialize timer1
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 31250;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 &= (0 << OCIE1A);  // DISable timer compare interrupt
  interrupts();             // enable all interrupts
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {  // button pressed
    ledsGreenState = true;
    ledsRedState = false;
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt -> LEDs start blinking
    playMelody();
    TIMSK1 &= (0 << OCIE1A);  // disable timer compare interrupt -> LEDs stop blinking
    ledsGreenState = true;
    ledsRedState = true;
  }
  else {   // button not pressed
    digitalWrite(ledsGreenPin, ledsGreenState);
    digitalWrite(ledsRedPin, ledsRedState);
  }
}

ISR(TIMER1_COMPA_vect) { // timer compare interrupt service routine -> to blink LEDs
  ledsGreenState = !ledsGreenState;
  ledsRedState = !ledsRedState;
  digitalWrite(ledsGreenPin, ledsGreenState);
  digitalWrite(ledsRedPin, ledsRedState);
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 's', 'g', 'a', 'v', 'b', 'C', 'D', 'E' };
  int tones[] = { 1915, 1700, 1519, 1432, 1352, 1275, 1136, 1073, 1014, 956, 852, 758 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 12; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playMelody() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }

    // pause between notes
    delay(tempo / 2);
  }
}
