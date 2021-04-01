#pragma once

struct FloatVector
{
	FloatVector() : x(0.0f), y(0.0f), z(0.0f){}
	FloatVector(float xx, float yy, float zz) : x(xx), y(yy), z(zz){}

    float x;
    float y;
    float z;
};

struct FloatRect
{
	FloatRect() : x0(0.0f), y0(0.0f), x1(0.0f), y1(0.0f) {}
	FloatRect(float xx0, float yy0, float xx1, float yy1) : x0(xx0), y0(yy0), x1(xx1), y1(yy1) {}

	float x0;
	float y0;
	float x1;
	float y1;
};

struct FloatBox
{
	FloatBox() : x0(0.0f), y0(0.0f), z0(0.0f), x1(0.0f), y1(0.0f), z1(0.0f) {}
	FloatBox(float xx0, float yy0, float zz0, float xx1, float yy1, float zz1) : x0(xx0), y0(yy0), z0(zz0), x1(xx1), y1(yy1), z1(zz1) {}

	float x0;
	float y0;
	float z0;
	float x1;
	float y1;
	float z1;
};

struct ShortVector
{
	ShortVector(short xx, short yy, short zz) : x(xx), y(yy), z(zz){}

    short x;
    short y;
    short z;
};

struct SByteVector
{
	SByteVector(char xx, char yy, char zz) : x(xx), y(yy), z(zz){}

    char x;
    char y;
    char z;
};

struct Point
{
	Point() : x(0), y(0){}
	Point(int xx, int yy) : x(xx), y(yy){}

	int x;
	int y;
};

struct FloatPoint
{
	FloatPoint() : x(-10000.0), y(-10000.0) {}
	FloatPoint(float xx, float yy) : x(xx), y(yy){}

	bool isUninitialised() {return x == -10000.0 && y == -10000.0;}
	bool isInitialised() {return !isUninitialised();}

	bool operator!=(FloatPoint& in){return x!=in.x || y!=in.y;}

	float x;
	float y;
};

