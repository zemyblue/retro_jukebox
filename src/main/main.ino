#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


SoftwareSerial playerSerial(5, 6);  //RX, TX
DFRobotDFPlayerMini dfPlayer;
void printDetail(uint8_t type, int value);

int pinPlay = 2;
int pinVolumeUp = 3;
int pinVolumeDown = 4;
int pinIsPlayingOut = 7;
int pinRedLED = 9;
int pinGreenLED = 10;
int pinBlueLED = 11;

int songCount = 0;

//==========================================
// Define LED Color State 
//==========================================

void setColor(int R, int G, int B) {
  analogWrite(pinRedLED, 255 - R);
  analogWrite(pinGreenLED, 255 - G);
  analogWrite(pinBlueLED, 255 - B);
}

void setWaitState() {
  setColor(255, 255, 255);
}

int playingStateBlueDirection = 1;
int playingStateBlue = 0;
void setPlayingAndEnableChangeState() {
  // change led yellow to white and white and yellow
  playingStateBlue = playingStateBlue + playingStateBlueDirection;
  if (playingStateBlue >= 80 || playingStateBlue <= 0) {
    playingStateBlueDirection = playingStateBlueDirection * -1;
  }
  setColor(255, 255, playingStateBlue);
}

void setPlayingState() {
  playingStateBlue = 0;
  playingStateBlueDirection = 1;
  setColor(255, 255, 0);
}

//==========================================
// Define management song play
//==========================================

unsigned long lastPlayTime = 0;
const unsigned long WaitNextPlayTime = 5000; // 5 seconds wait
bool isPlayButtonPushed = false;
bool isPlaying = false;
bool notPlaying = true; // the state of realtime pinIsPlayingOut


// plays the next random song among all songs.
void playNext() {
  int num = random(songCount);
  Serial.print(F("Play "));
  Serial.println(num);
  dfPlayer.playMp3Folder(num);
}

//==========================================
// Define Setup and Loop of Arduino
//==========================================

void setup() {
  pinMode(pinPlay, INPUT);  // Play
  pinMode(pinVolumeUp, INPUT);  // Volume up
  pinMode(pinVolumeDown, INPUT);  // Volum down 
  pinMode(pinIsPlayingOut, INPUT);  // Playing Status, Low means playing/High means no
  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);
  playerSerial.begin(9600);
  Serial.begin(9600);

  Serial.println();
  Serial.println(F("RetroJukeBox"));

  if (!dfPlayer.begin(playerSerial), false) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0);
    }
  }
  Serial.println(F("RetroJukeBox online."));

  dfPlayer.setTimeOut(500); // set serial network timeout

  dfPlayer.volume(20);  // set default volume. From 0 to 30

  randomSeed(analogRead(0));

  songCount = dfPlayer.readFileCounts();
  Serial.print(F("songCount: "));
  Serial.println(songCount);
  setWaitState();
}

void loop() {
  // if Volume up button is pushed
  if (digitalRead(pinVolumeUp) == HIGH) {
    dfPlayer.volumeUp();
    Serial.println(F("Volume Up"));
  }
  // if Volume down button is pushed
  if (digitalRead(pinVolumeDown) == HIGH) { 
    dfPlayer.volumeDown();
    Serial.println(F("Volume Down"));
  }

  // if Play button is pushed
  if (digitalRead(pinPlay) == HIGH) {
    if (!isPlayButtonPushed) {
      isPlayButtonPushed = true;
      setPlayingState();
    }
  } else if(isPlayButtonPushed) {
    Serial.println(F("Release Play Button"));
    if (millis() - lastPlayTime >= WaitNextPlayTime || lastPlayTime == 0) {
      playNext();

      lastPlayTime = millis();
      Serial.print(F("Time: "));
      Serial.println(lastPlayTime);
    }
    isPlayButtonPushed = false;
    setWaitState();
  }

  // check isPlaying and change led state
  notPlaying = digitalRead(pinIsPlayingOut);
  if (notPlaying == true && isPlaying == true) {
    isPlaying = false;
    setWaitState();
    Serial.println(F("Playing OFF"));
  } else if(notPlaying == false && isPlaying == false) {
    isPlaying = true;
    setPlayingState();
    Serial.println(F("Playing ON"));
  }

  // change led state if it is playing and the timeout time has elapsed.
  if (isPlaying) {
    if (millis() - lastPlayTime >= WaitNextPlayTime) {
      setPlayingAndEnableChangeState();
    }
  }

  delay(5);
}
