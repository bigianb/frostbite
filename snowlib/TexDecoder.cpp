#include "Texture.h"
#include "TexDecoder.h"
#include "DataUtil.h"
#include "GIFTag.h"
#include "Palette.h"

#pragma optimize( "", off )

Texture* TexDecoder::decode(const unsigned char* data, int ps2Addr)
{
	int finalw = DataUtil::getLEShort(data, 0);
	int finalh = DataUtil::getLEShort(data, 02);	

	int curIdx = DataUtil::getLEInt(data, 0x10);

	return decode(finalw, finalh, data, curIdx - ps2Addr);
}

Texture* TexDecoder::decode(int finalw, int finalh, const unsigned char* data, int curIdx)
{
	int sourcew = finalw;
	int sourceh = finalh;
	pixels = nullptr;
	pixelsLength = 0;

    GIFTag gifTag;
    gifTag.parse(data, curIdx);

    // This is basically heuristics. Writing a full GIF parser is complex and as the texture files are written by a tool,
    // we can safely make some assumptions about their structure.
    if (gifTag.nloop == 4) {

        int palw = DataUtil::getLEShort(data, curIdx + 0x30);
        int palh = DataUtil::getLEShort(data, curIdx + 0x34);

        curIdx += 0x50;
        GIFTag gifTag2;
        gifTag2.parse(data, curIdx);

        // 8 bit palletised
        Palette* palette = new Palette();
        palette->read(data + curIdx + 0x10, palw, palh);
        palette->unswizzle();

        int palLen = palw * palh * 4;
        curIdx += (palLen + 0x10);

        GIFTag* gifTag50 = new GIFTag();
        gifTag50->parse(data, curIdx);
        curIdx += 0x20;

        int dbw = (sourcew / 2 + 0x07) & ~0x07;
        int dbh = (sourceh / 2 + 0x07) & ~0x07;

        
		int totalRrw = 0;
		bool eop = false;
		// Need to find a better way than this.
        while (!eop || totalRrw < dbw) {
            GIFTag* gifTag3 = new GIFTag();
            gifTag3->parse(data, curIdx);

            int dimOffset = 0x10;

            int thisRrw = DataUtil::getLEShort(data, curIdx + dimOffset);
            int thisRrh = DataUtil::getLEShort(data, curIdx + dimOffset + 4);

            int startx = DataUtil::getLEShort(data, curIdx + dimOffset + 20);
            int starty = DataUtil::getLEShort(data, curIdx + dimOffset + 22);

            curIdx += gifTag.nloop * 0x10 + 0x10;
            readPixels32(data, palette, curIdx, startx, starty, thisRrw, thisRrh, dbw, dbh);
            curIdx += thisRrw * thisRrh * 4;

			totalRrw += thisRrw;
			eop = gifTag3->eop;
        }
        if (palLen != 64) {
            unswizzle8bpp(dbw * 2, dbh * 2);
            sourcew = dbw * 2;
            sourceh = dbh * 2;
        } else {
            sourcew = dbw;
            sourceh = dbh;
        }
        delete palette;        

    } else if (gifTag.nloop == 3) {
        GIFTag* gifTag2 = new GIFTag();
        gifTag2->parse(data, 0xC0);

        if (gifTag2->flg == 2) {
            // image mode
            readPixels32(data + 0xD0, finalw, finalh);
        }
    }
    Texture* texture = new Texture(finalw, finalh, sourcew, sourceh, pixels, sourcew * sourceh * 4);
    return texture;
}

void TexDecoder::unswizzle8bpp(int w, int h)
{
    unsigned char* unswizzled = new unsigned char[pixelsLength];

    int32_t* unswizzled32 = (int32_t*)unswizzled;
    int32_t* pixels32 = (int32_t*)pixels;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {

            int block_location = (y & (~0xf)) * w + (x & (~0xf)) * 2;
            int swap_selector = (((y + 2) >> 2) & 0x1) * 4;
            int posY = (((y & (~3)) >> 1) + (y & 1)) & 0x7;
            int column_location = posY * w * 2 + ((x + swap_selector) & 0x7) * 4;

            int byte_num = ((y >> 1) & 1) + ((x >> 2) & 2);     // 0,1,2,3

            int idx = block_location + column_location + byte_num;
            if (idx < pixelsLength) {
                unswizzled32[(y * w) + x] = pixels32[idx];
            }
        }
    }
    delete[] pixels;
    pixels = unswizzled;
}

void TexDecoder::readPixels32(const unsigned char* data, Palette* palette, int startOffset, int startx, int starty, int rrw, int rrh, int dbw, int dbh)
{
    if (palette->numEntries == 256) {
        int numDestPixels = dbh * dbw * 4;
        int widthPixels = dbw * 4;
        if (pixels == nullptr) {
			pixelsLength = numDestPixels*4;
            pixels = new unsigned char[pixelsLength];
        }
        int32_t* pixels32 = (int32_t*)pixels;
        int idx = startOffset;
        for (int y = 0; y < rrh && (y+starty) < dbh; ++y) {
            for (int x = 0; x < rrw; ++x) {
                int destIdx = (y+starty) * widthPixels + (x + startx) * 4;

                pixels32[destIdx++] = palette->getValue(data[idx++]);
                pixels32[destIdx++] = palette->getValue(data[idx++]);
                pixels32[destIdx++] = palette->getValue(data[idx++]);
                pixels32[destIdx] = palette->getValue(data[idx++]);
            }
        }

    } else {
        int numDestPixels = rrh * dbw;
        if (pixels == nullptr) {
            pixelsLength = numDestPixels*4;
            pixels = new unsigned char[pixelsLength];
        }
        int32_t* pixels32 = (int32_t*)pixels;
        int idx = startOffset;
        bool lowbit = false;
        for (int y = 0; y < rrh; ++y) {
            for (int x = 0; x < rrw; ++x) {
                int destIdx = (y + starty) * dbw + x + startx;
                if (!lowbit) {
                    pixels[destIdx] = palette->getValue(data[idx] >> 4 & 0x0F);
                } else {
                    pixels[destIdx] = palette->getValue(data[idx++] & 0x0F);
                }
                lowbit = !lowbit;
            }
        }
    }
}

    
void TexDecoder::readPixels32(const unsigned char* data, int w, int h)
{
    int numDestPixels = w * h;
    if (pixels == nullptr) {
        pixelsLength = numDestPixels*4;
        pixels = new unsigned char[pixelsLength];
    }
    memcpy(pixels, data, numDestPixels*4);
}