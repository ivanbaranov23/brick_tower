#pragma once
#include <string>

struct Pixel{
    unsigned char r, g, b, a = 255;
};

class Image{
public:
    unsigned char* data = NULL;
    size_t size = 0;
    std::string name;

    int w;
    int h;
    int channels;

    Image(const char* filename);
    Image(int w, int h, int ch = 4);
    Image(const Image& im);
    ~Image();

    bool read(const char* filename);
    bool write(const char* filename);

    Pixel get(int x, int y);
    void set(int x, int y, Pixel p);
    void set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);    
};