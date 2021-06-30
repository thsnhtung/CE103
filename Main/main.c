#include <REGX51.H>
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

sbit ledStart = P3^4;
sbit ledUart = P3^5;
sbit button = P3^6;
sbit led1 = P3^7;

unsigned char STATE_595_0 = 0x00;
unsigned char STATE_595_1 = 0x00;
unsigned char STATE_595_2 = 0x00;
unsigned char STATE_595_3 = 0x00;
////////////////////////////////
unsigned char r_data;
unsigned char START_BYTE= 0;
unsigned char MODE= 0;
unsigned char Count = 0 ;
////////////////////////////////
unsigned char isReceiveLength =  0;
unsigned char lengthOfEffect = 0 ;
unsigned char storage[100];
// unsigned char isStateOrDelay = 0 ; // = 0 means wait for led value, =1 means wait for delay value



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
	TH1 = 0xFD;		/* Load value for 9600 baud rate */ //F3
	SCON = 0x50;		/* Mode 1, reception enable */
	TR1 = 1;
	IE=0X90;		/* Start timer 1 */
}


void HardMode_Handler(unsigned char Data)
{
	if (Data >= 0 && Data < 256)
	{
		OUT_BYTE_LED(Data, Count);
		Count++;
		if (Count == 4)
		{
			Count = 0 ;					
			START_BYTE = 0 ;
			MODE = 0 ;							// clear mode
		}
	}
	else
	{
		// Out of range data 307,2
		// led on
	}
}

void CustomMode_Handler(unsigned char Data)
{
	if (isReceiveLength == 0)
	{
		lengthOfEffect = Data;
	}
	else
	{
		if (Count == lengthOfEffect * 2)
		{
			// nhan du roi, ignore
		}
		else
		{
			storage[Count] = Data; 
			Count++ ;
		}
		
	}
}


void Receive_Handler(unsigned char Data)
{
	TI = 0 ; 								// send uart de kiem tra
	SBUF = Data ; 
	Data -= 0x30;						// se xoa 
	while(TI ==0);
	TI = 0;
	if (START_BYTE == 0 && Data == 0)			// chua co du lieu va nhan duoc start_byte
	{
		Count = 0 ;
		ledStart = 0 ;
		START_BYTE = 1;
	}
	else if (START_BYTE == 1 && MODE == 0 && (Data > 0 && Data < 3))  // neu da start va mode chua co, thi check xem mode byte co dung format k __ mode 1 mode 2
	{
		MODE = Data;
	}
	else if (START_BYTE !=0 && MODE != 0)
	{
		if (MODE == 1 || MODE == 2 || MODE == 3)
		{
			HardMode_Handler(Data);
		}
		else if (MODE == 4)
			CustomMode_Handler(Data);
		else
		{
			// Turn on LED
			// should not reach here
		}
	}
	else				// chua hop sai
	{
		START_BYTE = 0 ;
		MODE = 0 ;
	}
	RI = 0;
}


void ISR_UART(void) interrupt 4
{
	if(RI == 1)           //Kiem tra xem la ngat nhan hay truyen
	{	
		ledUart = ~ledUart;
		r_data = SBUF;   				//nhan du lieu
		Receive_Handler(r_data);
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
	
	OUT_BYTE_LED(0x00,0);
	OUT_BYTE_LED(0x00,1);
	OUT_BYTE_LED(0x00,2);
	OUT_BYTE_LED(0x00,3);
  
	UART_Init();
   	 
	while(1)
	{
		 if (Count == lengthOfEffect * 2 && isReceiveLength == 1)					// neu da nhan duoc tin hieu thi 
		 {
			unsigned char i;
			for (i = 0 ; i < lengthOfEffect*2 ; i++)
			{
				if (i % 2 == 0)
				{
					OUT_BYTE_LED(storage[i],0);
				}
				else
				{
					custom_delay(storage[i],0);
				}
			}
		 }
	}
}