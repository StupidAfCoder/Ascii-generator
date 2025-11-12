#include <algorithm>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <ostream>
#include <vector>
#include "ascii-converter.hpp"

const std::vector<double>& getLinearized(std::vector<double>& values , double rf , double gf , double bf) {
    double r_lin{};
    double g_lin{};
    double b_lin{};
    values = {rf,gf,bf};
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
    values = {r_lin,g_lin,b_lin};
    return values;
}

void getResampling(std::vector<std::vector<double>>& l_src, std::vector<std::vector<double>>& l_cell , int width , int height , int cols , int rows){
        double block_w = width / (double) cols;
        double block_h = height / (double) rows;
        int sx{};
        int sy{};
        int ex{};
        int ey{};
        int temp1{};
        int temp2{};
        double sum{0.0};
        int count{0};
        for (int tr = 0 ; tr < rows ; tr++){
            for (int tc = 0; tc < cols ; tc++){
                sx = std::floor((tc * block_w));
                sy = std::floor((tr * block_h));
                temp1 = std::floor(((tc + 1) * block_w));
                temp2 = std::floor(((tr + 1) * block_h));
                ex = std::min(width , temp1);
                ey = std::min(height , temp2);

                if ( ex <= sx || ey <= sy ){
                    //Empty block
                    int cx = std::clamp(int((tc*block_w*(1 + block_w))*0.5) , 0 , width - 1);
                    int cy = std::clamp(int((tr*block_w*(1 + block_h))*0.5) , 0 , height - 1);
                    l_cell[tr][tc] = l_src[cy][cx];
                    continue;
                }

                for(int y = sy ; y < ey; y++){
                    for (int x = sx ; x < ex; x++){
                        sum += l_src[y][x];
                        count++;
                    }
                }
                l_cell[tr][tc] = sum / count;

            }
            sum = 0;
            count = 0;
        }
}

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
    std::vector<double> temp_array;
    //During The Main Program
    /*for (y ; y < height ; y++){
        for (x ; x < width ; x++){
            index = ( y * width + x) * desired_channels;
            int red = pixel_data[index];
            int green = pixel_data[index+1];
            int blue = pixel_data[index+2];
        }
    }*/
    int index{};
    double rf{};
    double gf{};
    double bf{};
    double linear_luminance{};
    std::vector<std::vector<double>> luminance(height, std::vector<double>(width,0.0));
    for (int y = 0 ; y < height ; y++){
        for (int x = 0; x < width ; x++){
            //Calculate Index and RGB values For One Pixel!!
            index = (y * width + x) * desired_channels;
            rf = pixel_data[index] / 255.0;
            gf = pixel_data[index+1] / 255.0;
            bf = pixel_data[index+2] / 255.0;
            //Have linearize for each value!!!! //Not Important but gets more sharp output :) (when sRGB)
            getLinearized(temp_array,rf,gf,bf);
            //Calculate Linear Luminance For That Pixel!!!
            linear_luminance = 0.2126 * temp_array[0] + 0.7152 * temp_array[1] + 0.0722 * temp_array[2];
            luminance[y][x] = linear_luminance;
        }
    }
    std::cout << "First Pixel luminance value: " << luminance[0][0] << std::endl;
    std::cout << "Last Pixel luminance value: " << luminance[height-1][width-1] << std::endl;
    float char_aspect = 0.55;
    int cols = 100;
    int rows = round(cols * (height / (double) width) * char_aspect);
    std::cout << "Rows: " << rows << " Columns: " << cols << std::endl;
    /*for (int i = 0; i < rows ; i++) {
        for (int j = 0; j < cols ; j++){
            std::cout << '#';
        }
        std::cout << '\n';
    }*/

    //Resampling the pixels to a block ( convert to a function !)
    std::vector<std::vector<double>> l_cell(rows , std::vector<double>(cols));
    getResampling(luminance, l_cell, width, height, cols, rows);

    std::cout << "First value of luminance for the block after resampling >> " << l_cell[0][0] << std::endl;

    //Add Brightness , Contrast , Gamma etc ..
    double brightness = 0.19;
    double contrast   = 1.12;
    double gamma      = 1.0;

    std::vector<std::vector<double>> added_values(rows , std::vector<double>(cols));
    for ( int i = 0 ; i < rows ; i++){
        for (int j = 0; j < cols ; j++){
            double value = l_cell[i][j];
            double v_contrast = (value - 0.5) * contrast + 0.5;
            double v_brightness = v_contrast + brightness;
            double v_clamp = std::clamp(v_brightness , 0.0 , 1.0);
            if (i == 4 && j == 4) {
                std::cout << "value=" << value 
                << " v_contrast=" << v_contrast
                << " v_brightness=" << v_brightness
                << " v_clamp=" << v_clamp
                << " gamma=" << gamma << '\n';
            }
            double v_value = std::pow(v_clamp , 1.0/gamma);
            added_values[i][j] = v_value;
        }
    }
    std::cout << "Check : " << added_values[4][4] << std::endl;

}   