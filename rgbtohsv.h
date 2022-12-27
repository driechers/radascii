#ifndef RGBTOHSV_H
#define RGBTOHSV_H

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

struct HSV
{
	double H;
	double S;
	double V;
};

struct HSV RGBToHSV(struct RGB rgb);

struct HSV rgb_to_hsv(int r, int g, int b);

#endif
