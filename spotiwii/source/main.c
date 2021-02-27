#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

// Wii Specific Includes

#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <network.h>
#include <fat.h>
#include <jpeg/jpgogc.h>

// implement the http response struct http/1.0

// IETF RFC 1945

// TODO: IMPLEMENT CURL 

static u32 *xfb;
static GXRModeObj *rmode;

ir_t ir;

extern char picdata[];
extern int piclength;

void init() {
    VIDEO_Init();
    WPAD_Init();
    WPAD_SetVRes(0, 640, 480);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    rmode = VIDEO_GetPreferredMode(NULL);

    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
}

void DrawHLine(int x1, int x2, int y, int color) {
    int i;
    y = 320 * y;
    x1 >>= 1;
    x2 >>= 1;
    for (i = x1; i <= x2; i++) {
        u32 *tmpfb = xfb;
        tmpfb[y+i] = color;
    }
}

void display_jpeg(JPEGIMG jpeg, int x, int y) {
    unsigned int *jpegout = (unsigned int *) jpeg.outbuffer;

    int i,j;
    int height = jpeg.height;
    int width = jpeg.width/2;
    
    for (i=0; i <= width; i++) 
        for (j=0; j<=height; j++) 
            xfb[(i+x)+320*(j+16+y)]=jpegout[i+width*j];
        

        free(jpeg.outbuffer);
    
}

void DrawVLine(int x, int y1, int y2, int color) {
    int i;
    x >>= 1;
    for (i = y1; i <= y2; i++) {
        u32 *tmpfb = xfb;
        tmpfb[x+(640*i)/2] = color;
    }
}

void DrawBox(int x1, int y1, int x2, int y2, int color) {
    DrawHLine(x1,x2,y1,color);
    DrawHLine(x1,x2,y2,color);
    DrawVLine(x1,y1,y2,color);
    DrawVLine(x2,y1,y2,color);
}

int main(int argc, char **argv) {
    JPEGIMG about;

    memset(&about, 0, sizeof(JPEGIMG));

    about.inbuffer = picdata;
    about.inbufferlength = piclength;
    JPEG_Decompress(&about);
    init();
    display_jpeg(about, 60, 100);
    return 0;
}