#include "em_device.h"
#include "em_chip.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"
#include "bsp_stk_buttons.h"

#define X_AXIS 0
#define Y_AXIS 1

#define	NEGATIVE 0
#define	POSITIVE 1

SegmentLCD_UpperCharSegments_TypeDef upperCharSegments[SEGMENT_LCD_NUM_OF_UPPER_CHARS];
SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

void delay() {
   for(int d=0;d<400000;d++);
}

uint8_t snake[37][2] = {0};
uint8_t length = 0;

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

struct food{
	  uint8_t x;
	  uint8_t y;
};

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize the LCD
   * (For this, we use the standard driver, located in files "segmentlcd.(c|h)"
   */
  SegmentLCD_Init(false);

  BSP_ButtonsInit();

  int8_t x = 1;
  int8_t y = 0;
  uint8_t dir = 0; // 0:negative, 1:positive
  uint8_t axis = X_AXIS; // 0:x, 1:y

  snake[0][0] = x;
  snake[0][1] = y;
  length = 0;

 struct food a;
 a.x = 3;
 a.y = 0;

 struct food prev;
 prev.x = x;
 prev.y = y;

 setSegment(a.x, a.y);

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
				  dir = !dir;
			  }else{
				  x -= 1;
				  y += 1;
			  }
		  }
		  axis = !axis;
	  }

	  else if(!BSP_ButtonGet(1)){ // Right turn
		  if(axis == X_AXIS){
			  if(dir == POSITIVE){
				  x += 1;
				  y += 1;
				  dir = !dir;
			  }else{
				  x -= 1;
				  y -= 1;
				  dir = !dir;
			  }
		  }else{
			  if(dir == POSITIVE){
				  x += 1;
				  y -= 1;
			  }else{
				  x += 1;
				  y += 1;
				  dir = !dir;
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
				  lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  x = 1;
			  }
		  }else{
			  if(x <= 0){
				  lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  x = 13;
			  }
		  }
	  }else{
		  if(dir == POSITIVE){
			  if(y <= 0){
				  lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  y = 3;
			  }
		  }else{

			  if(y >= 4){
				  lowerCharSegments[x/2].raw = 0;
				  SegmentLCD_LowerSegments(lowerCharSegments);
				  y = 1;
			  }
		  }
	  }


	  for(int i=length; i>0; i--){
		  snake[i][X_AXIS] = snake[i-1][X_AXIS];
		  snake[i][Y_AXIS] = snake[i-1][Y_AXIS];
	  }

	  snake[0][0] = x;
	  snake[0][1] = y;

	  if(x==a.x && y==a.y){
		  length++;
		  snake[length][0] = x;
		  snake[length][1] = y;
	  }

	  for(int i = 0; i<=length; i++){
		  setSegment(snake[i][0], snake[i][1]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }

	  delay();

	  for(int i = 0; i<=length; i++){
		  clearSegment(snake[i][0], snake[i][1]);
		  SegmentLCD_LowerSegments(lowerCharSegments);
	  }

	  prev.x = x;
	  prev.y = y;
  }
}
