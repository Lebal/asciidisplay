/*
 * 	startup.c
 *
 */
 
#define	STK_CTRL	((volatile unsigned int *) (0xE000E010))
#define	STK_LOAD	((volatile unsigned int *) (0xE000E014))
#define	STK_VAL		((volatile unsigned int *) (0xE000E018))



#define GPIO_E 0x40021000
#define GPIO_MODER  ((volatile unsigned int *) (GPIO_E))  
#define GPIO_OTYPER  ((volatile unsigned short *) (GPIO_E+0x4))  
#define GPIO_PUPDR ((volatile unsigned int *) (GPIO_E+0xC))  
#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO_E+0x10))  
#define GPIO_IDR_HIGH  ((volatile unsigned char *) (GPIO_E+0x11))  
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_E+0x14))  
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_E+0x15))  


#define B_E 0x40
#define B_SELECT 4
#define B_RW 2
#define B_RS 1

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );

void startup ( void )
{
asm volatile(
	" LDR R0,=0x2001C000\n"		/* set stack */
	" MOV SP,R0\n"
	" BL main\n"				/* call main */
	".L1: B .L1\n"				/* never return */
	) ;
}

 void delay_250ns(){
	 /*SystemCoreClock = 168000000*/
	 *STK_CTRL =  0;
	 *STK_LOAD = ((168/4) -1);
	 *STK_VAL = 0;
	 *STK_CTRL = 5;
	 while((*STK_CTRL & 0x10000) ==0){
		 }
	 
	 *STK_CTRL = 0;
	 
	 
 }
	 
void delay_milli(unsigned int ms){
	int i;
	
	#ifdef SIMULATOR
	ms = ms/100;
	ms++;
	#endif
	while (ms--){
#ifdef SIMULATOR
		for(i = 0; i<1; i++)
#else	
		for(i =0; i<1000;i++)
#endif
			{
			delay_250ns();
			delay_250ns();
			delay_250ns();
			delay_250ns();
			
			}
	}
}
	
void delay_mikro(unsigned int us){
	while(us--){
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		}
	
	
	}
 
 void app_Init(){
	*GPIO_MODER = 0x55555555;
	
	}
	
	
	
void ascii_ctrl_bit_set(unsigned char x){
	
	unsigned char c;
	c = *GPIO_ODR_LOW;
	c |= (B_SELECT | x);
	*GPIO_ODR_LOW = c;
	}
void ascii_ctrl_bit_clear(unsigned char x){
	unsigned char c;
	c = *GPIO_ODR_LOW;
	c = B_SELECT | (c & ~x);
	*GPIO_ODR_LOW = c;
	
	}
void ascii_write_controller(unsigned char c){
	ascii_ctrl_bit_set(B_E);
	*GPIO_ODR_HIGH = c;
	ascii_ctrl_bit_clear(B_E);
	delay_250ns();
	}
unsigned char ascii_read_read_controller(){
	unsigned char c;
	ascii_ctrl_bit_set(B_E);
	delay_250ns();
	delay_250ns();
	c = *GPIO_IDR_HIGH;
	ascii_ctrl_bit_clear(B_E);
	return c;
	
	}
	
void ascii_write_cmd(unsigned char command){
	ascii_ctrl_bit_clear(B_RS);
	ascii_ctrl_bit_clear(B_RW);
	
	ascii_write_controller(command);
	}
void ascii_write_data(unsigned char data){
	ascii_ctrl_bit_set(B_RS);
	ascii_ctrl_bit_clear(B_RW);
	
	ascii_write_controller(data);
	}

unsigned char ascii_read_status(){
	*GPIO_MODER &= 0x0000FFFF;
	*GPIO_MODER |= 0;
	
	ascii_ctrl_bit_clear(B_RS);
	ascii_ctrl_bit_set(B_RW);
	
	unsigned char rv;
	rv = ascii_read_read_controller();
	
	
	*GPIO_MODER &= 0x0000FFFF;
	*GPIO_MODER |= 0x55550000;
	return rv;
	
	}
unsigned char ascii_read_data(){
	*GPIO_MODER &= 0x0000FFFF;
	*GPIO_MODER |= 0;
	
	ascii_ctrl_bit_set(B_RS);
	ascii_ctrl_bit_set(B_RW);
	
	unsigned char rv;
	rv = ascii_read_read_controller();
	
	
	*GPIO_MODER &= 0x0000FFFF;
	*GPIO_MODER |= 0x55550000;
	return rv;
	
	}
	
void ascii_init(){
	while((ascii_read_status() & 0x80) == 0x80){}
	
	delay_mikro(8);
	ascii_write_cmd(60);
	delay_mikro(39);
	
	while((ascii_read_status() & 0x80) == 0x80){}
	
	delay_mikro(8);
	ascii_write_cmd(14);
	delay_mikro(39);
	
	while((ascii_read_status() & 0x80) == 0x80){}
	
	delay_mikro(8);
	ascii_write_cmd(1);
	delay_milli(2);
	while((ascii_read_status() & 0x80) == 0x80){}
	
	delay_mikro(8);
	ascii_write_cmd(4);
	delay_mikro(39);
	}
	
	
void ascii_write_char(unsigned char c){
	while((ascii_read_status() & 0x80) == 0x80){}
	
	delay_mikro(8);
	ascii_write_data(c);
	delay_mikro(43);
	
	}
void ascii_gotoxy(int x, int y){
	int adress = x -1;
	
	if(y == 2) adress += 0x40;
	ascii_write_cmd(0x80|adress);
	}
int main(int argc, char **argv)
{
	
	char *s;
	char test1[] = "Alfanumerisk";
	char test2[] = "Display - test";
	
	app_Init();
	ascii_init();
	ascii_gotoxy(1,1);
	s = test1;
	while(*s)
		ascii_write_char(*s++);
	s = test2;
	ascii_gotoxy(1,2);
	while(*s)
		ascii_write_char(*s++);
	return 0;
}

