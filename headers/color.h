#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
	inline Color(int _r = 255, int _g = 255, int _b = 255, int _a = 255)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

public:
	unsigned char r, g, b, a;
};

#endif