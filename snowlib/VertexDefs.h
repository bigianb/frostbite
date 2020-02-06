#pragma once

struct FloatVector
{
	FloatVector(){}
	FloatVector(float xx, float yy, float zz) : x(xx), y(yy), z(zz){}

    float x;
    float y;
    float z;
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
	Point(){}
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

