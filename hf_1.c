#include "em_device.h"
#include "em_chip.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"
#include "bsp_stk_buttons.h"

#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#include "caplesense.h"

#define X_AXIS 0
#define Y_AXIS 1

#define	NEGATIVE 0
#define	POSITIVE 1

#define LEFT 0
#define RIGHT 1

SegmentLCD_UpperCharSegments_TypeDef upperCharSegments[SEGMENT_LCD_NUM_OF_UPPER_CHARS];
SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

uint8_t snake[37][2] = {0};
uint8_t length = 0;

uint8_t foodIsSnake = 0;

struct segment{
	  int8_t x;
	  int8_t y;
};

void delay() {
   for(int d=0; d<500000; d++);
}

void setSegment(uint8_t x, uint8_t y){
	if(x != 14){
		if(x%2){
			if(!y) lowerCharSegments[x/2].a = 1;
			else if(y == 2) lowerCharSegments[x/2].raw |= (1<<6) | (1<<10);
			else if(y == 4) lowerCharSegments[x/2].d = 1;
		}else{
			if(y == 1) lowerCharSegments[x/2].f = 1;
			else if(y == 3) lowerCharSegments[x/2].e = 1;
		}
	}else{
		if(y == 1) lowerCharSegments[6].b = 1;
		else if(y == 3) lowerCharSegments[6].c = 1;
	}
}

void clearSegment(uint8_t x, uint8_t y){
	if(x != 14){
		if(x%2){
			if(!y) lowerCharSegments[x/2].a = 0;
			else if(y == 2) lowerCharSegments[x/2].raw &= ~(1<<6) & ~(1<<10);
			else if(y == 4) lowerCharSegments[x/2].d = 0;
		}else{
			if(y == 1) lowerCharSegments[x/2].f = 0;
			else if(y == 3) lowerCharSegments[x/2].e = 0;
		}
	}else{
		if(y == 1) lowerCharSegments[6].b = 0;
		else if(y == 3) lowerCharSegments[6].c = 0;
	}
}

void endOfGame(){
	SegmentLCD_AlphaNumberOff();
	while(1){
		for(int i = 11; i<=15; i++){	//LCD_SYMBOL_DP3
		SegmentLCD_Symbol(i, 1);
		}
		delay();

		for(int i = 11; i<=15; i++){	//LCD_SYMBOL_DP3
			SegmentLCD_Symbol(i, 0);
		}
		delay();
	}
}

void randomSegment(struct segment* s){

	s->x = rand()%15; // X-hez "igazitjuk" Y-t; Ha x paros, Y paratlan

	if(!(s->x%2)){ // ha X paros
		s->y = 2*(rand()%2)+1;
	}else{
		s->y = 2*(rand()%3);
	}
}

/* touch_turn(): check which side of the touch slider is being touched */
/* returns -1 if not touched, 0 if touched on the left side, 1 if on the right side */
int touch_turn()
{
	int touch_position = CAPLESENSE_getSliderPosition();
	if(touch_position == -1)
		return -1; /* in this case the snake does not turn */
	else if(touch_position > 23)
		return 1; /* turn right */
	else
		return 0; /* turn left */
}

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize the LCD
   * (For this, we use the standard driver, located in files "segmentlcd.(c|h)"
   */
  SegmentLCD_Init(false);


  /* Handling the capacitive touch sensor */
  CAPLESENSE_Init(false);

  uint8_t dir = POSITIVE; // 0:negative, 1:positive
  uint8_t axis = X_AXIS; // 0:x, 1:y

 struct segment food;
 struct segment head;
 struct segment prev;

 srand(1000101);
 randomSegment(&food);
 setSegment(food.x, food.y);

 snake[0][0] = head.x = 14;
 snake[0][1] = head.y = 2;
 length = 0;


  /* Infinite loop */
  while (1) {

	  int turn = touch_turn();

	  if(turn == LEFT){ // Left turn !BSP_ButtonGet(0)
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  head.x += 1;
				  head.y -= 1;
			  }else{
				  head.x -= 1;
				  head.y += 1;
			  }
		  }else{
			  if(dir == POSITIVE){
				  head.x -= 1;
				  head.y -= 1;
			  }else{
				  head.x += 1;
				  head.y += 1;
			  }
			  dir = !dir;
		  }
		  axis = !axis;
	  }

	  else if(turn == RIGHT){ // Right turn
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  head.x += 1;
				  head.y += 1;
			  }else{
				  head.x -= 1;
				  head.y -= 1;
			  }
			  dir = !dir;
		  }else{
			  if(dir == POSITIVE){
				  head.x += 1;
				  head.y -= 1;
			  }else{
				  head.x -= 1;
				  head.y += 1;
			  }
		  }
		  axis = !axis;
	  }

	  else{
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  head.x += 2;
			  }else{
				  head.x -= 2;
			  }
		  }else{
			  if(dir == POSITIVE){
				  head.y -= 2;
			  }else{
				  head.y += 2;
			  }
		  }
	  }

	  if(axis == X_AXIS){
		  if(dir == POSITIVE){
			  if(head.x > 14){
				  head.x = 1;
			  }
		  }else{
			  if(head.x < 0){
				  head.x = 13;
			  }
		  }
	  }else{
		  if(dir == POSITIVE){
			  if(head.y < 0){
				  head.y = 3;
			  }
		  }else{
			  if(head.y > 4){
				  head.y = 1;
			  }
		  }
	  }

	  prev.x = snake[length][X_AXIS];
	  prev.y = snake[length][Y_AXIS];

	  for(int i=length; i>0; i--){
		  snake[i][X_AXIS] = snake[i-1][X_AXIS];
		  snake[i][Y_AXIS] = snake[i-1][Y_AXIS];
	  }

	  snake[0][0] = head.x;
	  snake[0][1] = head.y;

	  if(head.x==food.x && head.y==food.y){
		  length++;
		  snake[length][X_AXIS] = prev.x;
		  snake[length][Y_AXIS] = prev.y;

		  foodIsSnake = 1;

		  while(foodIsSnake != 0){
			  foodIsSnake = 0;
		  	  randomSegment(&food);

		  	  for(int i=0; i<=length; i++){
		  		  if((snake[i][X_AXIS] == food.x) && (snake[i][Y_AXIS] == food.y)){
		  			  foodIsSnake = 1;
		  		  }
		  	  }
		  }
		  setSegment(food.x, food.y);
	  }

	  for(int i = 0; i<=length; i++){
		  setSegment(snake[i][X_AXIS], snake[i][Y_AXIS]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }

	  SegmentLCD_Number(length+1);
	  delay();

	  for(int i = 1; i<=length; i++){
		  if((head.x == snake[i][0]) && (head.y == snake[i][1])){
			  endOfGame();
		  }
	  }

	  for(int i = 0; i<=length; i++){
		  clearSegment(snake[i][X_AXIS], snake[i][Y_AXIS]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }
  }
}
