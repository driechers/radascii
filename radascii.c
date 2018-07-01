#define  _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <errno.h>
#include <ftw.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>
#include <opencv2/highgui/highgui_c.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[41m"
#define KGRN  "\x1B[42m"
#define KYEL  "\x1B[43m"
#define KBLU  "\x1B[44m"
#define KMAG  "\x1B[45m"
#define KCYN  "\x1B[46m"
#define KWHT  "\x1B[47m"
#define KCLFT "\x1B[80C"
#define KCUP  "\x1B[26A"
#define KCDN  "\x1B[26B"

#define COLOR_COUNT sizeof(pixelColors)/3/sizeof(int)

static char imageLocation[]="/tmp/tmp.XXXXXX";

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

void createImageLocation()
{
    mkdtemp(imageLocation);
}

static int rmFiles(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    remove(pathname);
    return 0;
}

void cleanImageLocation()
{
    nftw(imageLocation, rmFiles,10, FTW_DEPTH|FTW_MOUNT|FTW_PHYS);
}

// Copies to filePath which must be of size FILENAME_MAX
// location is 3 char location code used by national weather service
// frame 0 is most recent and it goes up to 7 aproximately 8 min apart
// TODO reduce webprofile by downloading only every 7 min
void downloadImage(char *filePath, char *location, int frame) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char url[50];
    char fileName[11];

    snprintf(url, 50,  "https://radar.weather.gov/lite/NCR/%s_%d.png", location, frame);
    snprintf(filePath, FILENAME_MAX, "%s/%s_%d.png", imageLocation, location, frame);

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filePath,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.47.0");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

void modeImage(IplImage *mode, IplImage *img)
{
    int h = img->height;
    int w = img->width;
    int mH = mode->height;
    int mW = mode->width;

    for(int row=0; row < mH; row++) {
        for(int col=0; col < mW; col++) {
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

    // TODO check if valid and supported location.
    char art[24*81];
    FILE *f = fopen(asciiFile, "r");
    if(!f) {
        printf("Could not open ascii art file for location.\n%s", strerror(errno));
        exit(-1);
    }

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

void drawFrame(char *location, int frame)
{
    char imagePath[FILENAME_MAX];
    //TODO consistent wait time between frames
    downloadImage(imagePath, location, frame);
    IplImage* radar = cvLoadImage(imagePath, CV_LOAD_IMAGE_COLOR);
    IplImage* mode = cvCreateImage(cvSize(80,24), IPL_DEPTH_8U, 3);

    //TODO Crop out border
    modeImage(mode, radar);
    char asciiFile[8];
    snprintf(asciiFile, 8, "%s.txt", location);
    drawRadar(asciiFile, mode);
    cvReleaseImage(&radar);
    cvReleaseImage(&mode);
}

void playAnimation(char *location)
{
    for(int frame=7; frame >= 0; frame--) {
        drawFrame(location, frame);
        puts(KCLFT);
        puts(KCUP);
    }
}

void usage()
{
    printf("Usage:\n\n"
           "radascii -s <site> [options]\n"
           "\tDefault behavior is to display the most recent radar image.\n"
           "\t-h Dispaly this menu.\n"
           "\t-a Play radar animation once.\n"
           "\t-l Continuously play radar loop.\n");
}

void getOptions(int *loop, int *animated, char **site, int argc, char **argv)
{
    int opt;

    while((opt=getopt(argc, argv, "lahs:")) != -1 )  {
        switch(opt) {
            case 'l':
                *loop = 1;
                break;
            case 'a':
                *animated = 1;
                break;
            case 'h':
                usage();
                exit(0);
            case 's':
                *site = optarg;
                break;
            default:
                usage();
                exit(-1);
        }
    }

    if(!*site) {
        usage();
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    //TODO Add more locations!
    int loop = 0;
    int animated = 0;
    char *site = NULL;

    getOptions(&loop, &animated, &site, argc, argv);

    // TODO cleanup when loop is killed by SIGTERM
    createImageLocation();

    if(loop) {
        while(1)
            playAnimation(site);
    }
    else if(animated) {
        playAnimation(site);
        puts(KCDN);
    }
    else {
        drawFrame(site, 0);
    }

    cleanImageLocation();

    return 0;
}
