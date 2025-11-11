#include <cmath>
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
    std::vector<Pixels> pixel_array;

    //During The Main Program
    /*for (y ; y < height ; y++){
        for (x ; x < width ; x++){
            index = ( y * width + x) * desired_channels;
            int red = pixel_data[index];
            int green = pixel_data[index+1];
            int blue = pixel_data[index+2];
        }
    }*/

    //Calculate Index and RGB values For One Pixel!!
    index = (y * width + x) * desired_channels;

    int red = pixel_data[index];
    int green = pixel_data[index+1];
    int blue = pixel_data[index+2];

    std::cout << "( " << red << green << blue << " )" << std::endl; 

    double rf = red / 255.0;
    double gf = green / 255.0;
    double bf = blue / 255.0;

    double r_lin{};
    double g_lin{};
    double b_lin{};

    std::cout << "( " << rf << " " << gf << " " << bf << " " << " )" << std::endl;

    //Have linearize for each value!!!! //Not Important but gets more sharp output :) (when sRGB)
    std::vector<double> values = {rf,gf,bf};
    for (int i = 0; i < 3 ; i++){
        if (values[i] <= 0.04045) {
            if (i == 0){
                r_lin = rf / 12.92; 
            } else if (i == 1) {
                g_lin = gf / 12.92;
            } else {
                b_lin = bf / 12.92;
            }
        } else {
            if (i == 0){
                r_lin = ((rf + 0.055) / 1.055);
                r_lin = std::pow(r_lin , 2.4); 
            } else if (i == 1) {
                g_lin = ((gf + 0.055) / 1.055);
                g_lin = std::pow(g_lin , 2.4); 
            } else {
                b_lin = ((bf + 0.055) / 1.055);
                b_lin = std::pow(b_lin , 2.4); 
            }
        }
    }

    std::cout << "( " << r_lin << " " << g_lin << " " << b_lin << " " << " )" << std::endl;

    //Calculate Linear Luminance For That Pixel!!!
    double linear_luminance = 0.2126 * r_lin + 0.7152 * g_lin + 0.0722 * b_lin;

    std::cout << linear_luminance << std::endl;

}   