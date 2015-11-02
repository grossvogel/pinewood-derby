#define STATE_IDLE 0
#define STATE_CALIBRATION 1
#define STATE_RACE 2
#define PIN_LED 5
#define PIN_BUTTON 6

#define MAX_RACE_TIME 45
#define SHADOW_RATIO 0.6
#define CALIBRATION_DELAY 100
#define CALIBRATION_CYCLES 50
#define DEBUG 1
int aSensorPins[] = {A1, A2, A3, A4};

int aBaselines[4];
double aTimes[4];
int aPlaces[4];
int state;
unsigned long start;


void setup ()
{
	  Serial.begin(9600);
	  setState(STATE_IDLE, "Ready");

	  pinMode (PIN_LED, OUTPUT);
	  pinMode (PIN_BUTTON, INPUT);
}

void loop ()
{
	while (true) {
		switch (state) {
			case STATE_IDLE:
				if (detectStartButton ()) {
                                        setState(STATE_CALIBRATION, "Calibrating");
				}
				break;
			case STATE_CALIBRATION:
				calibrate ();
				startRace ();
                                setState(STATE_RACE, "Racing");
				break;
			case STATE_RACE:
				watchRace ();
				printTimes ();
				setState(STATE_IDLE, "Ready");
				break;
			default:
				if (DEBUG) Serial.println("Unrecognized state?");
				break;
		}
	}
}

void setState(int iState, char* sState) {
  state = iState;
  Serial.print("state:");
  Serial.println(sState);
}

bool detectStartButton () {
        char c;
	if (digitalRead (PIN_BUTTON) == HIGH) {
		flashLED (1000);
		return true;
	} else if (Serial.available() > 0) {
                  while (Serial.available () > 0) {
                      c = Serial.read ();
                  }
                  return true;
        } else {
		delay (100);
		return false;
	}
}

void flashLED (int time) {
	digitalWrite (PIN_LED, HIGH);
	delay (time);
	digitalWrite (PIN_LED, LOW);
}

void calibrate () {
	for (int i = 0; i < 4; i++) {
		aBaselines[i] = 0;
	}

	for (int cycle = 0; cycle < CALIBRATION_CYCLES; cycle++) {
		for (int i = 0; i < 4; i++) {
			aBaselines[i] += analogRead (aSensorPins[i]);
		}
		delay (CALIBRATION_DELAY);
	}
	for (int i = 0; i < 4; i++) {
		aBaselines[i] = aBaselines[i] / CALIBRATION_CYCLES;
	}
	if (DEBUG) {
		for (int i = 0; i < 4; i++) {
			Serial.print("Baseline value for lane ");
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.println(aBaselines[i]);
		}
	}
}

void startRace () {
	flashLED (100);
	delay (600);
	flashLED (100);
	delay (600);
	start = millis ();
	flashLED (1000);
}

void watchRace () {
	long unsigned currentTime;
	double elapsed;
	int reading;
	int finished = 0;

	for (int i = 0; i < 4; i++) {
		aTimes[i] = -1.00;
		aPlaces[i] = -1;
	}
	
	while (true) {
		currentTime = millis ();
		elapsed = ((double) currentTime - start) / 1000;

		for (int i = 0; i < 4; i++) {
			if (aTimes[i] < 0.00) {
				reading = analogRead (aSensorPins[i]);
				if (reading <= (aBaselines[i] * SHADOW_RATIO)) {
					aTimes[i] = elapsed;
					aPlaces[finished] = i;
					finished++;
					if (DEBUG) {
						Serial.print ("*** FINISHED: Lane ");
						Serial.print (i + 1);
						Serial.println (" ***");
					}
				}
			}
		}

		if (finished >= 4 || elapsed > MAX_RACE_TIME || digitalRead(PIN_BUTTON) == HIGH) {
			break;
		}
	}

}

void printTimes () {
	Serial.println("");
	Serial.println("--------------------------------");
	Serial.println("        Race Results!");
	Serial.println("--------------------------------");
	Serial.println("");
	if (aPlaces[0] >= 0) {
		Serial.print ("In first place: Lane ");
		Serial.println (aPlaces[0] + 1);
		Serial.print ("   time: ");
		Serial.println (aTimes[0]);
	} 
	if (aPlaces[1] >= 0) {
		Serial.print ("In second place: Lane ");
		Serial.println (aPlaces[1] + 1);
		Serial.print ("   time: ");
		Serial.println (aTimes[1]);
	} 
	if (aPlaces[2] >= 0) {
		Serial.print ("In third place: Lane ");
		Serial.println (aPlaces[2] + 1);
		Serial.print ("   time: ");
		Serial.println (aTimes[2]);
	} 
	if (aPlaces[3] >= 0) {
		Serial.print ("In fourth place: Lane ");
		Serial.println (aPlaces[3] + 1);
		Serial.print ("   time: ");
		Serial.println (aTimes[3]);
	} else {
		Serial.println ("And some of us need to work on making it all the way down the track!");
	}
	Serial.println("");
        Serial.println("<<<");
}


