#include "io.h"
#include "fb.h"

void main()
{
    uart_init();
    fb_init();

    //HD 1920x1080
    //void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
    //

    drawRect(90,90,200,145,0x03,0);

    //drawCircle(960,540,250,0x0e,0);
    //drawCircle(960,540,50,0x13,1);

    //drawPixel(250,250,0x0e);

    //drawChar('O',500,500,0x05);
    drawString(100,100,"emexOS rpi4b",0x0f);
    drawString(100,110,"Hello world!",0x0f);
    drawString(100,120,"Shell loaded",0x0f);
    //in future there will be a real shell
    drawString(100,130," >",0x0f);

    drawRect(-1,1040,1921,1080,0xFF,1);

    //drawLine(100,500,350,700,0x0c);

    while (1);
}
