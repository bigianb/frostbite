#pragma once

class Texture
{
public:
    Texture(int logw, int logh, int w, int h, unsigned char* dataIn, int dataLengthIn)
    {
		logicalWidth = logw;
		logicalHeight = logh;
        widthPixels = w;
        heightPixels = h;
        dataLength = dataLengthIn;
        data = dataIn;
    }

    ~Texture()
    {
        delete data;
    }

	int logicalWidth;
	int logicalHeight;

    int widthPixels;
    int heightPixels;

    int dataLength;
    unsigned char* data;
};
