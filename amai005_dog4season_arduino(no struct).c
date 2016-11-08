#include <SoftwareSerial.h>
#include <Wire.h>
#include <math.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

char incomingByte;

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix.begin(0x70);  // pass in the address
  randomSeed(analogRead(0));
}

struct Dog{
  uint16_t  c; //col
  uint16_t  r; //row
};

/*
struct Ball{
  uint16_t x0; //head
  uint16_t x1; //tail
  uint16_t dimention; //length
};*/

//harcode generated pattern
/*uint8_t ball_arrayT[5][8] ={{0,0,0,0,0,0,0}, //beginning array (empty)
                            {0,64,0,0,0,2,0,0}, //first
                            {0,12,0,12,0,0,0,0}, //second
                            {0,0,0,0,0,268,0,268}, //third 
                            {0,330,0,330,0,330,0,330} }; //last*/

static const uint8_t PROGMEM
  empty[] = {0,0,0,0,0,0,0},
  first[] ={0,64,0,0,0,2,0,0},
  second[] ={0,12,0,12,0,0,0,0},
  third[] ={0,0,0,0,0,192,0,192},  
  last[] ={0,145,0,145,0,145,0,145};

/* Global variables
*
*/

struct Dog dog_array[4];
//struct Ball *ball_array[4];
bool game_status;
bool pressA;
char keypad_input;
unsigned char ball_a;
char ball_b;


/*start screen function
 * loops until user press 'A'
 */
void start_screen(){
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(LED_GREEN);
  int8_t x;
  for (x=7; x>=-125; x--) {
    if(Serial.available()){
        keypad_input = Serial.read();
        Serial.println(keypad_input);
        if(keypad_input == 'A'){pressA = true; return;}
    }
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("A Dog for all Seasons");
    matrix.writeDisplay();
    delay(80);
  }
  //if 'A' isn't pressed, loop again.
  if(!pressA){start_screen();}
}


//vertical coordinate of dog
enum y_states{y_start, y_wait, y_jump, y_stay} y_state;

void y_sm(){
  static unsigned char cnt;
  
  //action
  switch(y_state){
    case y_jump:
      for(int i = 0; i < 4; ++i){
        //row
        dog_array[i].r -= 1;
      }
      break;
    case y_stay:
      break;
    default:
    case y_start:
      //set initial
      for(int i = 0; i < 4; ++i){
        //row
        dog_array[i].r = (2*i) + 1;
      }
      break;
  }
  //transition
  switch(y_state){
    case y_jump:
      y_state = y_stay;
      break;
     case y_stay:
      //the dog will stay in the air for 2 cnts
      if(cnt > 2){
        y_state = y_start;
      }
      ++cnt;
      break;
    default:
    case y_start:
        if(keypad_input== 'D'){y_state = y_jump;}
      cnt = 0;
      break;
  }
}


//horizontal coordinate of dog
enum x_states{x_start, x_wait} x_state;

void x_sm(){
  //transition
  switch(x_state){
    case x_wait:
      //if the user presses any of the 6 button that causes the dog to move in the horizonal direction
      //valid keys: 1,3, 4,6, 7,9, *,#
      if(keypad_input == 'C' || keypad_input == '1' || keypad_input == '3' || keypad_input == '4' || keypad_input == '6' || keypad_input == '7' 
          || keypad_input == '9' ||keypad_input == '*' || keypad_input == '#'){
           //Serial.write(keypad_input);
           if(keypad_input == 'C') //reset to default
           {
              //Serial.write(keypad_input);
              x_state = x_start;
           }
           else if(keypad_input == '3')//dog0_r
           {
              if((dog_array[0].c + 1) < 7){
                dog_array[0].c += 1;
              }
           }
           else if(keypad_input == '1')//dog0_l
          {
            if((dog_array[0].c - 1) > 0){
              dog_array[0].c -= 1; 
            }
          }
          else if(keypad_input == '6')//dog1_r
          {
            if((dog_array[1].c + 1) < 7){
              dog_array[1].c += 1;
            }
          }
          else if(keypad_input == '4')//dog1_l
          {
            if((dog_array[1].c - 1) > 0){
              dog_array[1].c -= 1; 
            }
          }
          else if(keypad_input == '9')//dog2_r
          {
            if((dog_array[2].c + 1) < 7){
              dog_array[2].c += 1;
            }
          }
          else if(keypad_input == '7')//dog2_l
          {
            if((dog_array[2].c - 1) > 0){
              dog_array[2].c -= 1; 
            }
          }
          else if(keypad_input == '#')//dog3_r
          {
            if((dog_array[3].c + 1) < 7){
              dog_array[3].c += 1;
            }
          }
          else if(keypad_input == '*')//dog3_l
          {
            if((dog_array[3].c - 1) > 0){
              dog_array[3].c -= 1; 
            }
          }
      }
      break;
    default:
    break;
  }
  //action
  switch(x_state){
    case x_start:
      for(int i = 0; i < 4; ++i){
          //col
          dog_array[i].c = 1;
        }
        x_state = x_wait;
     default:
      break;
  }
}

/*enum ball_states{ball_start, ball_generate, ball_wait, } ball_state;

void ball_sm(){
  
  
 
}*/

enum paint_states{paint_start} paint_state;

void paint_sm(){
  static int x;
  static int y;

  //transition
  switch(paint_state){
    case paint_start:
    default:
      break;
  }
  
  //action
  switch(paint_state){
    case paint_start:
      //paint dogs
      matrix.clear();
      for(int i = 0; i < 4; ++i){
        matrix.drawPixel(dog_array[i].c, dog_array[i].r, LED_YELLOW);
      }
      /*for(int i = 0; i < 4; ++i){
        //matrix.drawLine(ball_array[i].x0, (2*i) + 1, ball_array[i].x1, (2*i) + 1, LED_RED);
        //matrix.drawLine(-1, (2*i) + 1, -1, (2*i) + 1, LED_RED);
      }*/
      default:
      break;
  }
}

enum ball_states{ball_start, ball_update} ball_state;

void paint_ball(){
  switch(ball_state){
    
    case ball_update:
       //draw and shift balls
      delay(100);
      if(ball_a == 0){
            matrix.drawBitmap(ball_b, 0, empty, 8, 8, LED_RED);
      }
      else if(ball_a == 1){
            matrix.drawBitmap(ball_b, 0, first, 8, 8, LED_RED);
            //Serial.println("1");
      }
      else if(ball_a == 2){
            matrix.drawBitmap(ball_b, 0, second, 8, 8, LED_RED);
            //Serial.println("2");
      }
      else if(ball_a==3){
            matrix.drawBitmap(ball_b, 0, third, 8, 8, LED_RED);
      }
      else if(ball_a==4){
            matrix.drawBitmap(ball_b, 0, last, 8, 8, LED_RED);
      }
      --ball_b;
      //Serial.println(ball_b);
      if(ball_b < -8){
            ball_b = 8;
            ++ball_a;
      }
      if(ball_a > 4){ball_a = 0;}
      //matrix.writeDisplay();
      break;
    default:
    case ball_start:
      ball_a = 0;
      ball_b = 8;
      ball_state = ball_update;
      break;
  }
}

void conditionD(){
  unsigned char dec = 0;
  
  if(ball_a == 1){
        if((dog_array[0].c == ball_b) && (dog_array[0].r == 1)){
      ++dec;
        }
      if((dog_array[2].c == ball_b) && (dog_array[2].r == 5)){
          ++dec;
        }
      }
      else if(ball_a == 2){
           if((dog_array[0].c == ball_b +4) && (dog_array[0].r == 1)){
      ++dec;
        }
      else if((dog_array[0].c == ball_b +5) && (dog_array[0].r == 1)){
      ++dec;
      } 
      if((dog_array[1].c == ball_b +4) && (dog_array[1].r == 3)){
      ++dec;
        }
      else if((dog_array[1].c == ball_b +5) && (dog_array[1].r == 3)){
      ++dec;
      } 
      }
      else if(ball_a==3){
            if((dog_array[2].c == ball_b +2) && (dog_array[2].r == 5)){
      ++dec;
        }
      else if((dog_array[2].c == ball_b +3) && (dog_array[2].r == 5)){
      ++dec;
      } 
      if((dog_array[3].c == ball_b +2) && (dog_array[2].r == 7)){
      ++dec;
        }
      else if((dog_array[3].c == ball_b +3) && (dog_array[2].r == 7)){
      ++dec;
      } 
      }
      else if(ball_a==4){
            if((dog_array[0].c == ball_b +1) && (dog_array[0].r == 1)){
      ++dec;
        }
      else if((dog_array[0].c == ball_b +3) && (dog_array[0].r == 1)){
      ++dec;
      } 
    else if((dog_array[0].c == ball_b +8) && (dog_array[0].r == 1)){
      ++dec;
      } 
      if((dog_array[1].c == ball_b +1) && (dog_array[1].r == 3)){
      ++dec;
        }
      else if((dog_array[1].c == ball_b +3) && (dog_array[1].r == 3)){
      ++dec;
      } 
    else if((dog_array[1].c == ball_b +8) && (dog_array[1].r == 3)){
      ++dec;
      } 
      if((dog_array[2].c == ball_b +1) && (dog_array[2].r == 5)){
      ++dec;
        }
      else if((dog_array[2].c == ball_b +3) && (dog_array[2].r == 5)){
      ++dec;
      } 
    else if((dog_array[2].c == ball_b +8) && (dog_array[2].r == 5)){
      ++dec;
      } 
      if((dog_array[3].c == ball_b +1) && (dog_array[2].r == 7)){
      ++dec;
        }
      else if((dog_array[3].c == ball_b +3) && (dog_array[3].r == 7)){
      ++dec;
      } 
    else if((dog_array[3].c == ball_b +8) && (dog_array[3].r == 7)){
      ++dec;
      } 
      
      }
	if(dec != 0){
		 Serial.write(dec + 4);
	}
}
void conditionI(){
  unsigned char inc = 0;
  
  if(ball_a == 1){
        if((dog_array[0].c == ball_b) && (dog_array[0].r == 0)){
      ++inc;
        }
      if((dog_array[2].c == ball_b) && (dog_array[2].r == 4)){
          ++inc;
        }
      }
      else if(ball_a == 2){
           if((dog_array[0].c == ball_b +4) && (dog_array[0].r == 0)){
      ++inc;
        }
      else if((dog_array[0].c == ball_b +5) && (dog_array[0].r == 0)){
      ++inc;
      } 
      if((dog_array[1].c == ball_b +4) && (dog_array[1].r == 2)){
      ++inc;
        }
      else if((dog_array[1].c == ball_b +5) && (dog_array[1].r == 2)){
      ++inc;
      } 
      }
      else if(ball_a==3){
            if((dog_array[2].c == ball_b +2) && (dog_array[2].r == 4)){
      ++inc;
        }
      else if((dog_array[2].c == ball_b +3) && (dog_array[2].r == 4)){
      ++inc;
      } 
      if((dog_array[3].c == ball_b +2) && (dog_array[2].r == 6)){
      ++inc;
        }
      else if((dog_array[3].c == ball_b +3) && (dog_array[2].r == 6)){
      ++inc;
      } 
      }
      else if(ball_a==4){
            if((dog_array[0].c == ball_b +1) && (dog_array[0].r == 0)){
      ++inc;
        }
      else if((dog_array[0].c == ball_b +3) && (dog_array[0].r == 0)){
      ++inc;
      } 
    else if((dog_array[0].c == ball_b +8) && (dog_array[0].r == 0)){
      ++inc;
      } 
      if((dog_array[1].c == ball_b +1) && (dog_array[1].r == 2)){
      ++inc;
        }
      else if((dog_array[1].c == ball_b +3) && (dog_array[1].r == 2)){
      ++inc;
      } 
    else if((dog_array[1].c == ball_b +8) && (dog_array[1].r == 2)){
      ++inc;
      } 
      if((dog_array[2].c == ball_b +1) && (dog_array[2].r == 4)){
      ++inc;
        }
      else if((dog_array[2].c == ball_b +3) && (dog_array[2].r == 4)){
      ++inc;
      } 
    else if((dog_array[2].c == ball_b +8) && (dog_array[2].r == 4)){
      ++inc;
      } 
      if((dog_array[3].c == ball_b +1) && (dog_array[2].r == 6)){
      ++inc;
        }
      else if((dog_array[3].c == ball_b +3) && (dog_array[3].r == 6)){
      ++inc;
      } 
    else if((dog_array[3].c == ball_b +8) && (dog_array[3].r == 6)){
      ++inc;
      } 
      
      }
	if(inc != 0){
		 //Serial.write(inc);
	}
}

//main
void loop() {
  //intitial conditions
  matrix.setRotation(3);
  game_status = true;
  pressA = false;

  start_screen();
  
  while(game_status){
    if(Serial.available()){
        keypad_input = Serial.read();
    }
    if(keypad_input != 0x09 && keypad_input != 0x0A){
      y_sm();
      x_sm();
      //ball_sm();
      paint_sm();
      paint_ball();
      matrix.writeDisplay();
      conditionI();
      conditionD();
      delay(50);
      keypad_input = '\0';
    }
    else{
      game_status = false;
    }
  }
  delay(800);

  /*condition:
   * if condition == 1, win()
   * if condition == 0, lose()
   */

  }