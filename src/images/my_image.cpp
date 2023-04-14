//#include "stdafx.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "my_image.h"


Image::Image(const char* filename){
    if (read(filename))
        size = w * h * channels;
}

Image::Image(int w, int h, int ch):w(w), h(h), channels(ch){
    size = w * h * channels;
    data = new unsigned char[size];
}

Image::Image(const Image& im):Image(im.w, im.h, im.channels){
    memcpy(data, im.data, im.size);
}

Image::~Image(){
    stbi_image_free(data);
}

bool Image::read(const char* filename){
    if (data != nullptr){
        stbi_image_free(data);
        data = nullptr;
    }
    name = filename;
    data = stbi_load(filename, &w, &h, &channels, 0);
    return data != NULL;
}

bool Image::write(const char* filename){
    return stbi_write_png(filename, w, h, channels, data, w*channels);
}


Pixel Image::get(int x, int y){
    int id = (y * w + x)*channels;
    return Pixel{
        data[id], 
        data[id+1],
        data[id+2],
        data[id+3]
        };
}

void Image::set(int x, int y, Pixel p){
    int id = (y * w + x)*channels;
    data[id] = p.r;
    data[id+1] = p.g;
    data[id+2] = p.b;
    data[id+3] = p.a;
}

void Image::set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    int id = (y * w + x)*channels;
    data[id] = r;
    data[id+1] = g;
    data[id+2] = b;
    data[id+3] = a;
}