// Simon Says

#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>

void playButton(int button[6], int offset=0, int duration=250);

// Variables to make the program more readable
int led1=0,led2=1,freq=2,r=3,g=4,b=5,start = 0, level = 1, turn = 2, win = 3, lose = 4, redButton = 0, blueButton = 1, yellowButton = 2, greenButton = 3;
int state = 0;

// Each array contains information for the two NEOPixels to use, frequency to play, and the RGBs to use to play the "button"
int red[6] = {3,4,440,255,0,0};
int blue[6] = {0,1,164,0,0,255};
int yellow[6] = {8,9,261,255,255,0};
int green[6] = {5,6,329,0,255,0};
int colors[4] = {red,blue,yellow,green}; //For ease of playing the buttons during startup and sequence

int sequence[20],sequenceCT=0; //Track the random sequence during gameplay

//General gameplay variables
int startUpButton=0, playerPressed, playerCT, winSequenceOffset = 0;
bool buttonPressed = false, buttonReleased = false, playSound;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  CircuitPlayground.begin();

  state = start;
}

void loop() {
  if(state == start){
    startUp();
  }else if(state == level){
    showSequence();
  }else if(state == turn){
    playersTurn();
  }else if(state == win){
    gameWin();
  }else{
    gameOver();
  }
  playSound = CircuitPlayground.slideSwitch();
}

void startUp(){
    CircuitPlayground.clearPixels();
    playButton(colors[startUpButton%4]);
    startUpButton++;
    if(CircuitPlayground.leftButton() || CircuitPlayground.rightButton()){
      blinkLights(3);
      state = level;
    }
    delay(500);
}

void showSequence(){
    delay(250);
    if(sequenceCT == 7){
      state = win;
      sequenceCT=0;
    }else{
      sequence[sequenceCT] = random(4);
      sequenceCT++;
      for(int i = 0; i < sequenceCT; i++){
        CircuitPlayground.clearPixels();
        delay(250);
        playButton(colors[sequence[i]]);
        delay(750);
      }
      state = turn; 
      playerCT = 0;
    }
}

void playersTurn(){
    CircuitPlayground.clearPixels();

    if(buttonPressed && CircuitPlayground.readCap(3) < 100 && CircuitPlayground.readCap(2) < 100 && CircuitPlayground.readCap(0) < 100 && CircuitPlayground.readCap(1) < 100 && CircuitPlayground.readCap(12) < 100 && CircuitPlayground.readCap(6) < 100 && CircuitPlayground.readCap(9) < 100 && CircuitPlayground.readCap(10) < 100 ){
      buttonReleased = true;
      buttonPressed = false;
    }

    if(CircuitPlayground.readCap(3) > 100 || CircuitPlayground.readCap(2) > 100){
      playButton(blue);
      playerPressed = blueButton;
      buttonPressed = true;
    }else if(CircuitPlayground.readCap(0) > 100 || CircuitPlayground.readCap(1) > 100){
      playButton(red);
      playerPressed = redButton;
      buttonPressed = true;
    }else if(CircuitPlayground.readCap(12) > 100 || CircuitPlayground.readCap(6) > 100){
      playButton(green);
      playerPressed = greenButton;
      buttonPressed = true;
    }else if(CircuitPlayground.readCap(9) > 100 || CircuitPlayground.readCap(10) > 100){
      playButton(yellow);
      playerPressed = yellowButton;
      buttonPressed = true;
    }
    
    if(buttonReleased){
      if(playerPressed != sequence[playerCT]){
        state = lose;
        sequenceCT=0;
      }else if(playerPressed == sequence[playerCT]){
        if(playerCT + 1 == sequenceCT){
          state = level;  
        }else{
          playerCT++;
        }
      }
      buttonPressed = false;
      buttonReleased = false;
    }
    delay(200);
}

void gameOver(){
  CircuitPlayground.clearPixels();
  for(int i = 5; i < 10; i++){
    CircuitPlayground.setPixelColor(i, 255,0,0);
    CircuitPlayground.setPixelColor(9 - i, 255,0,0);
    if(playSound){
      CircuitPlayground.playTone(500 - i * 35, 250);
    }else{
      delay(250);
    }
  }
  blinkLights(5);
  state = start;
}

void gameWin(){
  winSequenceOffset++;
  //CircuitPlayground.clearPixels();
  playButton(blue,winSequenceOffset,40);
  playButton(red,winSequenceOffset,40);
  playButton(green,winSequenceOffset,40);
  playButton(yellow,winSequenceOffset,40);
  CircuitPlayground.setPixelColor((2 + winSequenceOffset)%10,255,255,255);
  CircuitPlayground.setPixelColor((7 + winSequenceOffset)%10, 255,255,255);
  if(winSequenceOffset >= 50){
    blinkLights(3);
    state = start;
  }
}
void playButton(int button[6], int offset, int duration){ 
      CircuitPlayground.setPixelColor((button[led1] + offset)%10, button[r],button[g],button[b]);
      CircuitPlayground.setPixelColor((button[led2] + offset)%10, button[r],button[g],button[b]);
      if(playSound){
        CircuitPlayground.playTone(button[freq], duration);
      }else{
        delay(duration);
      }
}
void blinkLights(int amt){
    for(int t = 0; t < amt; t++){
      for(int i = 0; i < 10; i++){
        CircuitPlayground.setPixelColor(i, 255,255,255);
      }
      delay(200);
      CircuitPlayground.clearPixels();
      delay(100);
    }  
}

