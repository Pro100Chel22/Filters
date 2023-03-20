#pragma once

#include <vector>
#include "lodepng/lodepng.h"
#include "lodepng/lodepng.cpp"

struct VecUI2
{
    unsigned int x = 0, y = 0;
};

struct Color
{
    unsigned char r = 0, g = 0, b = 0, a = 255;
};

class Image
{
public:
    Image()
    {
        creatImage(0, 0);
    }

    unsigned int loadFromFile(const std::string& nameFile)
    {
        creatImage(0, 0);
        unsigned int error = lodepng::decode(img, width, height, nameFile);

        r.resize(img.size() / 4);
        g.resize(img.size() / 4);
        b.resize(img.size() / 4);
        for (int i = 0; i < img.size(); i+=4)
        {
            r[i / 4] = img[i + 0];
            g[i / 4] = img[i + 1];
            b[i / 4] = img[i + 2];
        }

        return error;
    }
    unsigned int saveToFile(const std::string& nameFile)
    {
        unsigned int error = lodepng::encode(nameFile, img, width, height);
        return error;
    }

    VecUI2 getSize()
    {
        return VecUI2{ width, height };
    }
    Color getPixel(VecUI2 pos)
    {
        return getPixel(pos.x, pos.y);
    }
    Color getPixel(unsigned int x, unsigned int y)
    {
        Color pixel;

        pixel.r = img[4 * y * width + 4 * x + 0];
        pixel.g = img[4 * y * width + 4 * x + 1];
        pixel.b = img[4 * y * width + 4 * x + 2];
        pixel.a = img[4 * y * width + 4 * x + 3];

        return pixel;
    }

    std::vector<unsigned char>::iterator getIterPixel(unsigned int x, unsigned int y)
    {
        return img.begin() + 4 * y * width + 4 * x;
    }
    std::vector<unsigned short>::iterator getIterR(unsigned int x, unsigned int y)
    {
        return r.begin() + y * width + x;
    }
    std::vector<unsigned short>::iterator getIterG(unsigned int x, unsigned int y)
    {
        return g.begin() + y * width + x;
    }
    std::vector<unsigned short>::iterator getIterB(unsigned int x, unsigned int y)
    {
        return b.begin() + y * width + x;
    }

    void creatImage(unsigned int width, unsigned int height, Color defaultPixel = Color{ 0, 0, 0, 255 })
    {
        this->width = width;
        this->height = height;
        img.resize(4 * width * height);
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                setPixel(x, y, defaultPixel);
            }
        }
    }
    void setPixel(VecUI2 pos, Color pixel)
    {
        setPixel(pos.x, pos.y, pixel);
    }
    void setPixel(unsigned int x, unsigned int y, Color pixel)
    {
        img[4 * y * width + 4 * x + 0] = pixel.r;
        img[4 * y * width + 4 * x + 1] = pixel.g;
        img[4 * y * width + 4 * x + 2] = pixel.b;
        img[4 * y * width + 4 * x + 3] = pixel.a;
    }

private:
    std::vector<unsigned char> img;
    std::vector<unsigned short> r;
    std::vector<unsigned short> g;
    std::vector<unsigned short> b;
    
    unsigned int width, height;

};