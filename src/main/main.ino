#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial playerSerial(10, 11);  //RX, TX
DFRobotDFPlayerMini dfPlayer;
void printDetail(uint8_t type, int value);

int pinPlay = 2;
int pinVolumeUp = 3;
int pinVolumeDown = 4;
int pinIsPlayingOut = 12;
int songCount = 0;

void setup() {
  pinMode(pinPlay, INPUT);  // Play
  pinMode(pinVolumeUp, INPUT);  // Volume up
  pinMode(pinVolumeDown, INPUT);  // Volum down 
  pinMode(pinIsPlayingOut, INPUT);  // Playing Status, Low means playing/High means no
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
}

unsigned long lastPlayTime = 0;
const unsigned long WaitNextPlayTime = 10000; // 10 seconds wait
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

void loop() {
  if (digitalRead(pinVolumeUp) == HIGH) {
    dfPlayer.volumeUp();
    Serial.println(F("Volume Up"));
  }
  if (digitalRead(pinVolumeDown) == HIGH) { 
    dfPlayer.volumeDown();
    Serial.println(F("Volume Down"));
  }

  if (digitalRead(pinPlay) == HIGH) {
    if (!isPlayButtonPushed) {
      isPlayButtonPushed = true;
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
  }

  // check isPlaying
  notPlaying = digitalRead(pinIsPlayingOut);
  if (notPlaying == true && isPlaying == true) {
    isPlaying = false;
    Serial.println(F("Playing OFF"));
  } else if(notPlaying == false && isPlaying == false) {
    isPlaying = true;
    Serial.println(F("Playing ON"));
  }

  delay(1);
}
