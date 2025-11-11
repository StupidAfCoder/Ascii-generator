#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <ostream>
#include <vector>
#include "ascii-converter.hpp"

struct Pixels {
    int red,blue,green;
};

void Ascii::generate(){
    std::cout << getPath() << std::endl;
    int n{};
    int width{};
    int height{};
    int desired_channels = 3;
    //Here x and y are the values for the array position and n is channels for the photo!
    unsigned char* pixel_data = stbi_load( image_path.c_str(),&width,&height,&n, desired_channels);

    if (!pixel_data) {
        std::cerr << "Failed to load image!" << stbi_failure_reason() << std::endl;
    }

    std::cout << "Width: " << width << " Height: " << height << " Channels: " << n << std::endl;

    int y = 0;
    int x = 0;
    int index{};

    /*for (y ; y < height ; y++){
        for (x ; x < width ; x++){
            index = ( y * width + x) * desired_channels;
            int red = pixel_data[index];
            int green = pixel_data[index+1];
            int blue = pixel_data[index+2];
            std::cout << "( " << red << green << blue << " ) || ";                
        }
    }*/

    index = (y * width + x) * desired_channels;

    int red = pixel_data[index];
    int green = pixel_data[index+1];
    int blue = pixel_data[index+2];

    std::cout << "( " << red << green << blue << " )" << std::endl; 
}   