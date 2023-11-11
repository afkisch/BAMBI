#include "em_device.h"
#include "em_chip.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"
#include "bsp_stk_buttons.h"

#include "stdlib.h"
#include "time.h"

#define X_AXIS 0
#define Y_AXIS 1

#define	NEGATIVE 0
#define	POSITIVE 1

SegmentLCD_UpperCharSegments_TypeDef upperCharSegments[SEGMENT_LCD_NUM_OF_UPPER_CHARS];
SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

uint8_t snake[37][2] = {0};
uint8_t length = 0;

//uint8_t lcd_digit[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};

struct segment{
	  uint8_t x;
	  uint8_t y;
};

void delay() {
   for(int d=0;d<150000;d++);
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

/*void dispNumber(uint8_t number){
	if(number > 999){
		upperCharSegments[3].raw = lcd_digit[number/1000];
		number %= 1000;
	}
	if(number > 99){
		upperCharSegments[2].raw = lcd_digit[number/100];
		number %= 100;
	}
	if(number > 9){
		upperCharSegments[1].raw = lcd_digit[number/10];
		number %= 10;
	}
	upperCharSegments[0].raw = lcd_digit[number];
}*/

void endOfGame(){
	while(1){ // Ez igy nem jo, de Gyurcsany ezt hozta
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

	setSegment(s->x, s->y);
	//SegmentLCD_LowerSegments(lowerCharSegments);
}

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize the LCD
   * (For this, we use the standard driver, located in files "segmentlcd.(c|h)"
   */
  SegmentLCD_Init(false);

  BSP_ButtonsInit();

  int8_t x;
  int8_t y;
  uint8_t dir = POSITIVE; // 0:negative, 1:positive
  uint8_t axis = X_AXIS; // 0:x, 1:y

 struct segment food;
 struct segment head;
 struct segment prev;


 srand(time(0));

 randomSegment(&food);

 snake[0][0] = x = head.x = 14;
 snake[0][1] = y = head.y = 2;
 length = 0;


  /* Infinite loop */
  while (1) {

	  if(!BSP_ButtonGet(0)){ // Left turn
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  x += 1;
				  y -= 1;
			  }else{
				  x -= 1;
				  y += 1;
			  }
		  }else{
			  if(dir == POSITIVE){
				  x -= 1;
				  y -= 1;
			  }else{
				  x += 1;
				  y += 1;
			  }
			  dir = !dir;
		  }
		  axis = !axis;
	  }

	  else if(!BSP_ButtonGet(1)){ // Right turn
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  x += 1;
				  y += 1;
			  }else{
				  x -= 1;
				  y -= 1;
			  }
			  dir = !dir;
		  }else{
			  if(dir == POSITIVE){
				  x += 1;
				  y -= 1;
			  }else{
				  x -= 1;
				  y += 1;
			  }
		  }
		  axis = !axis;
	  }

	  else{
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  x+=2;
			  }else{
				  x-=2;
			  }
		  }else{
			  if(dir == POSITIVE){
				  y -= 2;
			  }else{
				  y += 2;
			  }
		  }
	  }

	  if(axis == X_AXIS){
		  if(dir == POSITIVE){
			  if(x >= 14){
				  //lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  x = 1;
			  }
		  }else{
			  if(x <= 0){
				  //lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  x = 13;
			  }
		  }
	  }else{
		  if(dir == POSITIVE){
			  if(y <= 0){
				  //lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  y = 3;
			  }
		  }else{

			  if(y >= 4){
				  //lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  y = 1;
			  }
		  }
	  }

	  prev.x = snake[length][X_AXIS];
	  prev.y = snake[length][Y_AXIS];

	  for(int i=length; i>0; i--){
		  snake[i][X_AXIS] = snake[i-1][X_AXIS];
		  snake[i][Y_AXIS] = snake[i-1][Y_AXIS];
	  }

	  snake[0][0] = x;
	  snake[0][1] = y;

	  if(x==food.x && y==food.y){
		  length++;
		  snake[length][X_AXIS] = prev.x;
		  snake[length][Y_AXIS] = prev.y;

		  randomSegment(&food);
	  }

	  for(int i = 0; i<=length; i++){
		  setSegment(snake[i][X_AXIS], snake[i][Y_AXIS]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }

	  SegmentLCD_Number(length+1);
	  delay();

	  for(int i = 1; i<=length; i++){
		  if((x == snake[i][0]) && (y == snake[i][1])){
			  endOfGame();
		  }
	  }

	  for(int i = 0; i<=length; i++){
		  clearSegment(snake[i][X_AXIS], snake[i][Y_AXIS]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }
  }
}
