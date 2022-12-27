// Shamelessly copied from https://www.programmingalgorithms.com/algorithm/rgb-to-hsv/c

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

static double Min(double a, double b) {
	return a <= b ? a : b;
}

static double Max(double a, double b) {
	return a >= b ? a : b;
}

struct HSV RGBToHSV(struct RGB rgb) {
	double delta, min;
	double h = 0, s, v;

	min = Min(Min(rgb.R, rgb.G), rgb.B);
	v = Max(Max(rgb.R, rgb.G), rgb.B);
	delta = v - min;

	if (v == 0.0)
		s = 0;
	else
		s = delta / v;

	if (s == 0)
		h = 0.0;

	else
	{
		if (rgb.R == v)
			h = (rgb.G - rgb.B) / delta;
		else if (rgb.G == v)
			h = 2 + (rgb.B - rgb.R) / delta;
		else if (rgb.B == v)
			h = 4 + (rgb.R - rgb.G) / delta;

		h *= 60;

		if (h < 0.0)
			h = h + 360;
	}

	struct HSV hsv;
	hsv.H = h;
	hsv.S = s;
	hsv.V = v / 255;

	return hsv;
}

struct HSV rgb_to_hsv(int r, int g, int b)
{
	struct RGB rgb = { r, g, b };
	return RGBToHSV(rgb);
}
