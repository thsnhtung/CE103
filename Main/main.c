#include <REGX52.H>
//Khai bao ket noi IC 74595

sbit IDX0_IC595_SHCP = P0^0;	  //xung clock
sbit IDX0_IC595_DS = P0^1;	  // chan data
sbit IDX0_IC595_STCP = P0^2;	// chan chot du lieu 

sbit IDX1_IC595_SHCP = P0^3;	  //xung clock
sbit IDX1_IC595_DS = P0^4;	  // chan data
sbit IDX1_IC595_STCP = P0^5;	// chan chot du lieu 

sbit IDX2_IC595_SHCP = P1^0;	  //xung clock
sbit IDX2_IC595_DS = P1^1;	  // chan data
sbit IDX2_IC595_STCP = P1^2;	// chan chot du lieu 

sbit IDX3_IC595_SHCP = P1^3;	  //xung clock
sbit IDX3_IC595_DS = P1^4;	  // chan data
sbit IDX3_IC595_STCP = P1^5;	// chan chot du lieu 

sbit button = P3^0;
sbit led1 = P3^1;
unsigned char STATE_595_0 = 0x00;
unsigned char STATE_595_1 = 0x00;
unsigned char STATE_595_2 = 0x00;
unsigned char STATE_595_3 = 0x00;
////////////////////////////////
unsigned char n;
unsigned char START_BYTE=1;
unsigned char MODE=1;





void OUT_BYTE_LED(unsigned char value, unsigned char idx)
{
	if (idx == 0)
	{
		unsigned char i;
		STATE_595_0 = value;
		for (i = 0 ; i < 8 ; i++)
		{
			IDX0_IC595_DS = !(value & (0x80 >>i));
			IDX0_IC595_SHCP = 1;
			IDX0_IC595_SHCP = 0 ;
		}
		IDX0_IC595_STCP = 1 ;
		IDX0_IC595_STCP = 0 ; 
	}
	else if (idx == 1)
	{
		unsigned char i;
		STATE_595_1 = value;
		for (i = 0 ; i < 8 ; i++)
		{
			IDX1_IC595_DS = !(value & (0x80 >>i));
			IDX1_IC595_SHCP = 1;
			IDX1_IC595_SHCP = 0 ;
		}
		IDX1_IC595_STCP = 1 ;
		IDX1_IC595_STCP = 0 ; 
	}
	else if (idx == 2)
	{
		unsigned char i;
		STATE_595_2 = value;
		for (i = 0 ; i < 8 ; i++)
		{
			IDX2_IC595_DS = !(value & (0x80 >>i));
			IDX2_IC595_SHCP = 1;
			IDX2_IC595_SHCP = 0 ;
		}
		IDX2_IC595_STCP = 1 ;
		IDX2_IC595_STCP = 0 ; 
	}
	
	else 
	{
		unsigned char i;
		STATE_595_3 = value;
		for (i = 0 ; i < 8 ; i++)
		{
			IDX3_IC595_DS = !(value & (0x80 >>i));
			IDX3_IC595_SHCP = 1;
			IDX3_IC595_SHCP = 0 ;
		}
		IDX3_IC595_STCP = 1 ;
		IDX3_IC595_STCP = 0 ; 
	}
	
	
}



void OUT_BIT_LED(unsigned char idx, unsigned char idx_595)
{
	if (idx_595 == 0)
	{
		STATE_595_0 |= (0x01 << idx) ; 
		OUT_BYTE_LED(STATE_595_0, idx_595);
	}
	else if (idx_595 == 1)
	{
		STATE_595_1 |= (0x01 << idx) ; 
		OUT_BYTE_LED(STATE_595_1, idx_595);
	}
	else if (idx_595 == 2)
	{
		STATE_595_2 |= (0x01 << idx) ; 
		OUT_BYTE_LED(STATE_595_2, idx_595);
	}
	else 
	{
		STATE_595_3 |= (0x01 << idx) ; 
		OUT_BYTE_LED(STATE_595_3, idx_595);
	}
}
 /////////////////////////////
//khoi tao
void UART_Init()
{
	TMOD = 0x20;		/* Timer 1, 8-bit auto reload mode */
	TH1 = 0xFD;		/* Load value for 9600 baud rate */
	SCON = 0x50;		/* Mode 1, reception enable */
	TR1 = 1;
	IE=0X90;		/* Start timer 1 */
}

/////////////////////////////
 void check()
{
  if(START_BYTE!=0 && MODE!=0)
  {
   OUT_BIT_LED(START_BYTE,MODE);
  }

  

}
//ngat nhan UART
//unsigned char temp=0;
unsigned char dem=0;
void ISR_UART(void) interrupt 4
{
 
 if(RI == 1)           //Kiem tra xem la ngat nhan hay truyen
 {	
    n = SBUF;   //nhan du lieu
    RI = 0;
	if(n==0)
	{
	  if (START_BYTE==0)
	  {
	  START_BYTE=1;
	  }
	  else if (START_BYTE=1)
	  { 
	  	 if(n<4)
		 {  MODE=n;  }
		 else { START_BYTE=0;MODE=0;}
	  
	  
	   }
	
	}
	else
	{
	 if(START_BYTE==0){OUT_BIT_LED(1,1);}
	 else {
	 if(MODE==0 && n<4){MODE=n;}
	 else if(MODE==0 && n>=4){OUT_BIT_LED(1,1);}
	 
	 if(MODE!=0)
	 { if(dem<=4)
	  { OUT_BIT_LED(n,MODE);
	     dem++;
    	}
		else 
		{
		 OUT_BIT_LED(1,1);
		 dem=0;
		}
	 
	 }
	 
	 
	   }

	
	
	
	}
 }

 else if (TI==1)
 {
 	TI=0;
 }
}

void main()
{
	
//	// INIT 32 LED
	OUT_BYTE_LED(0x00,0);
	OUT_BYTE_LED(0x00,1);
	OUT_BYTE_LED(0x00,2);
	OUT_BYTE_LED(0x00,3);
	// CLEAR 32 LED
//	
	OUT_BYTE_LED(0x00,0);
	OUT_BYTE_LED(0x00,1);
	OUT_BYTE_LED(0x00,2);
	OUT_BYTE_LED(0x00,3);
//	
//	button = 1 ;
//	n = 0;
   UART_Init();
   	 
	while(1)
	{
		 
	}
}