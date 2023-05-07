#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial playerSerial(10, 11);  //RX, TX
DFRobotDFPlayerMini dfPlayer;
void printDetail(uint8_t type, int value);

int pinPlay = 2;
int pinVolumeUp = 3;
int pinVolumeDown = 4;
int songCount = 0;

void setup() {
  pinMode(2, INPUT);  // Play
  pinMode(3, INPUT);  // Volume up
  pinMode(4, INPUT);  // Volum down 
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
bool isPlayButtonPushed = false;
const unsigned long WaitNextPlayTime = 10000; // 10 seconds wait

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
      int num = random(songCount);
      Serial.print(F("Play "));
      Serial.println(num);
      dfPlayer.playMp3Folder(num);
      lastPlayTime = millis();
      Serial.print(F("Time: "));
      Serial.println(lastPlayTime);
    }
    isPlayButtonPushed = false;
  }
}
