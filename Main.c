#include <string.h>
#include "debug.h"
//EPD
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"	
//GUI
#include "GUI_Paint.h"
#include "Fonts/fonts.h"
#include "qrcode.h"

unsigned char BlackImage[4000];//Define canvas space  

const unsigned char gImage_battery[56] = { /* 0X00,0X01,0X1F,0X00,0X0E,0X00, */
0XFF,0XFF,0XFF,0XF0,0X80,0X00,0X00,0X10,0XBD,0XEF,0X78,0X1E,0XBD,0XEF,0X78,0X12,
0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X12,
0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X12,0XBD,0XEF,0X78,0X1E,
0X80,0X00,0X00,0X10,0XFF,0XFF,0XFF,0XF0,};

const unsigned char gImage_network[32] = { /* 0X00,0X01,0X10,0X00,0X10,0X00, */
0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X1C,0X38,0X30,0X0C,0X47,0XE2,0X0C,0X30,
0X10,0X08,0X03,0XC0,0X06,0X60,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
};

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main( void )
{
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    Delay_Init();
    USART_Printf_Init( 115200 );
    EPD_GPIO_Init();	   //EPD GPIO  initialization
	
    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://blog.csdn.net/hydfxy2018");
    printf( "qrcode.size:%d\r\n", qrcode.size);
    while( 1 )
    {
	Paint_NewImage(BlackImage, EPD_WIDTH, EPD_HEIGHT, 0, WHITE);  
	printf("Partial refresh\r\n");
	Paint_SelectImage(BlackImage);
	EPD_HW_Init_GUI(); //EPD init GUI
	EPD_SetRAMValue_BaseMapWhite();//EPD Clear
	Paint_Clear(WHITE);
	PAINT_TIME sPaint_time;
	sPaint_time.Hour = 12;
	sPaint_time.Min = 34;
	sPaint_time.Sec = 56;
	Paint_DrawTime(0, 20, &sPaint_time, &Font16, WHITE, BLACK);
	Paint_DrawString_EN(0, 60, "iot-lorawan", &Font16, WHITE, BLACK); //14*20
	Paint_DrawBitMap_Paste(gImage_battery,91,0,31,14,WHITE);
	Paint_DrawBitMap_Paste(gImage_network,0,0,16,16,WHITE);

	for (uint8_t y = 0; y < qrcode.size; y++) {
		for (uint8_t x = 0; x < qrcode.size; x++) {
			if (qrcode_getModule(&qrcode, x, y)) {
				Paint_DrawFillRect(0 + x * 3,
						150 +  y * 3, 3, 3);
			} 
		}
		printf("\n");
	}

	UBYTE num = 20;
	for (;;) {
		sPaint_time.Sec = sPaint_time.Sec + 1;
		if (sPaint_time.Sec == 60) {
			sPaint_time.Min = sPaint_time.Min + 1;
			sPaint_time.Sec = 0;
			if (sPaint_time.Min == 60) {
				sPaint_time.Hour =	sPaint_time.Hour + 1;
				sPaint_time.Min = 0;
				if (sPaint_time.Hour == 24) {
					sPaint_time.Hour = 0;
					sPaint_time.Min = 0;
					sPaint_time.Sec = 0;
				}
			}
		}
		Paint_ClearWindows(0, 20, 0 + Font16.Width * 7, 20 + Font16.Height, WHITE);
		Paint_DrawTime(0, 20, &sPaint_time, &Font16, WHITE, BLACK);
		num = num - 1;
		if(num == 0) {
			break;
		}
		Delay_Ms(500);//Analog clock 1s
		EPD_PartDisplay(BlackImage);//display image
	}
	EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
	Delay_Ms(2000); //2s	

	EPD_HW_Init_GUI(); //EPD init GUI
	EPD_Display(BlackImage); //display image
	EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!		
    }
}



