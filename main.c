#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>

#include "third_party/oled/OLED_1in5_rgb.h"
#include "third_party/gui/GUI_Paint.h"

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(int argc, char *argv[])
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    printf("Starting program.. \n");
    if(DEV_ModuleInit() != 0)
    {
        return -1;
    }

 	if(USE_IIC) {
		printf("Only USE_SPI, Please revise DEV_Config.h !!!\r\n");
		return -1;
	}
	
	printf("OLED Init...\r\n");
	OLED_1in5_rgb_Init();
	DEV_Delay_ms(500);	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = (OLED_1in5_RGB_WIDTH*2) * OLED_1in5_RGB_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize + 300)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 0, BLACK);	
	Paint_SetScale(65);
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

    return 0;
}
