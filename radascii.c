#include <opencv2/highgui/highgui_c.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[41m"
#define KGRN  "\x1B[42m"
#define KYEL  "\x1B[43m"
#define KBLU  "\x1B[44m"
#define KMAG  "\x1B[45m"
#define KCYN  "\x1B[46m"
#define KWHT  "\x1B[47m"

#define COLOR_COUNT sizeof(pixelColors)/3/sizeof(int)

int pixelColors[][3] =
{
    {255, 255, 255}, // Nothing
    {231, 233,   4}, // DB05
    {244, 159,   1}, // DB10
    {244,   0,   3}, // DB15
    {  2, 253,   2}, // DB20
    {  1, 197,   1}, // DB25
    {  0, 142,   0}, // DB30
    {  2, 248, 253}, // DB35
    {  0, 188, 229}, // DB40
    {  0, 139, 253}, // DB45
    {  0,   0, 253}, // DB50
};

// TODO: smart colors ie check for 256 support
char *escapes[] =
{
    KNRM,
    KCYN,
    KCYN,
    KBLU,
    KGRN,
    KGRN,
    KGRN,
    KYEL,
    KYEL,
    KYEL,
    KRED,
};

void modeImage(IplImage *mode, IplImage *img)
{
    int h = img->height;
    int w = img->width;
    int mH = mode->height;
    int mW = mode->width;

    for(int row=0; row < mH; row++) {
        for(int col=0; col < mW; col++) {
//TODO: add blank to mode and weight such that color takes priority
            int count[COLOR_COUNT];
            memset(count, 0, COLOR_COUNT*sizeof(int));

            for(int y=row*h/mH; y < (row+1)*h/mH; y++) {
                for(int x=col*w/mW; x < (col+1)*w/mW; x++) {
                    uint8_t b = img->imageData[3*(y*w+x)];
                    uint8_t g = img->imageData[3*(y*w+x)+1];
                    uint8_t r = img->imageData[3*(y*w+x)+2];

                    for(int color=0; color < COLOR_COUNT; color++) {
                        if(pixelColors[color][0] == b && 
                           pixelColors[color][1] == g &&
                           pixelColors[color][2] == r)
                            count[color]++;
                    }
                }
            }

            int max=0;
            int maxi=0;
            for(int i=0; i<COLOR_COUNT;i++) {
                if(count[i] > max){
                    max = count[i];
                    maxi = i;
                }
            }

            if(max != 0) {
                mode->imageData[3*(row*mW+col)] = pixelColors[maxi][0];
                mode->imageData[3*(row*mW+col)+1] = pixelColors[maxi][1];
                mode->imageData[3*(row*mW+col)+2] = pixelColors[maxi][2];
            }
            else {
                mode->imageData[3*(row*mW+col)] = 0;
                mode->imageData[3*(row*mW+col)+1] = 0;
                mode->imageData[3*(row*mW+col)+2] = 0;
            }
        }
    }
}

//TODO directly set DBZ value to avoid reprocessing of color
void drawRadar(char *asciiFile, IplImage *mode)
{
    int mH = mode->height;
    int mW = mode->width;

    char art[24*81];
    FILE *f = fopen(asciiFile, "r");
    fread(art, 24*81, 1, f);
    fclose(f);

    for(int row=0; row < mH; row++) {
        for(int col=0; col < mW; col++) {
            uint8_t b = mode->imageData[3*(row*mW+col)];
            uint8_t g = mode->imageData[3*(row*mW+col)+1];
            uint8_t r = mode->imageData[3*(row*mW+col)+2];

            char *escape = "";
            for(int color=0; color < COLOR_COUNT; color++) {
                if(pixelColors[color][0] == b && 
                   pixelColors[color][1] == g &&
                   pixelColors[color][2] == r) {
                    escape = escapes[color];
                    break;
                }
            }

            printf("%s%c"KNRM, escape, art[row*81+col]);
        }
        printf("\n");
    }
}

int main(void)
{
    //TODO Download image with libcurl
    //TODO grab location from config
    IplImage* radar = cvLoadImage("DVN_0.png", CV_LOAD_IMAGE_COLOR);
    IplImage* mode = cvCreateImage(cvSize(80,24), IPL_DEPTH_8U, 3);
    if(radar == NULL)
            return -1;

    //TODO Crop out border
    modeImage(mode, radar);
    //TODO Add more locations!
    drawRadar("dvn.txt", mode);

    //cvShowImage("Image", radar);
    //cvShowImage("mode", mode);
    cvWaitKey(0);
    cvReleaseImage(&radar);
    cvReleaseImage(&mode);

    return 0;
}
