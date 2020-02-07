#pragma once

class Texture;
class Palette;

class TexDecoder
{
public:
	Texture* decode(const unsigned char* data, int len);
	Texture* decode(int finalw, int finalh, const unsigned char* data, int curIdx, int len);

private:
    void readPixels32(const unsigned char* data, Palette* palette, int startOffset, int startx, int starty, int rrw, int rrh, int dbw, int dbh);
    void readPixels32(const unsigned char* data, int w, int h);

    void unswizzle8bpp(int w, int h);
    unsigned char* pixels;
    int pixelsLength;
};
