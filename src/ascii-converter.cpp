#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "ascii-converter.hpp"
#include "stb_image.h"


void Ascii::generate(){
    std::cout << image_path << std::endl;
    int n{};
    int width{};
    int height{};
    //Here x and y are the values for the array position and n is channels for the photo!
    unsigned char* pixel_data = stbi_load( image_path.c_str(),&width,&height,&n, 3);

    if (!pixel_data) {
        std::cerr << "Failed to load image!" << stbi_failure_reason() << std::endl;
    }

    int y = 0;
    int x = 0;

    index = ()
}