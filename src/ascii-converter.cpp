#include <algorithm>
#include <array>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
        for (int tr = 0 ; tr < rows ; tr++){
            for (int tc = 0; tc < cols ; tc++){
                double sx_f = tc * block_w;
                double sy_f = tr * block_h;
                double ex_f = (tc + 1) * block_w;
                double ey_f = (tr + 1) * block_h;
                sx = std::floor(sx_f);
                sy = std::floor(sy_f);
                ex = std::min(width , (int)std::floor(ex_f));
                ey = std::min(height , (int) std::floor(ey_f));

                if ( ex <= sx || ey <= sy ){
                    //Empty block
                    int cx = std::clamp(int(std::round((sx_f + ex_f) * 0.5)) , 0 , width - 1);
                    int cy = std::clamp(int(std::round((sy_f + ey_f) * 0.5)) , 0 , height - 1);
                    l_cell[tr][tc] = l_src[cy][cx];
                    continue;
                }

                double sum{0.0};
                int count{0};
                for(int y = sy ; y < ey; y++){
                    for (int x = sx ; x < ex; x++){
                        sum += l_src[y][x];
                        count++;
                    }
                }
                l_cell[tr][tc] = sum / (double) count;

            }
        }
}

std::array<int, 4> computeBoxForCrop(const std::vector<std::vector<double>>& luminance , int width , int height , double mean_Y){
    double threshold = mean_Y - (0.15 * mean_Y);
    int pad = std::clamp(width / 200 , 4 , 64);
    int minX = width; int minY = height; int maxX = -1; int maxY = -1;

    for (int y = 0; y < height ; y++){
        for (int x = 0; x < width; x++){
            bool keep = false;

            if (!keep) {
                if (luminance[y][x] > threshold){
                    keep = true;
                }
            }

            if (keep) {
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
            }
        }
    }

    if (maxX < minX) {
        return {0 , 0 , width - 1 , height - 1};
    }

    minX = std::max(0 , minX - pad);
    minY = std::max(0 , minY - pad);
    maxX = std::min(width - 1 , maxX + pad);
    maxY = std::min(height - 1 , maxY + pad);

    return {minX , minY , maxX , maxY};
}

std::vector<std::vector<double>> MainCrop(const std::vector<std::vector<double>>& luminance , const std::array<int, 4>& box){
    int minX = box[0]; int minY = box[1]; int maxX = box[2]; int maxY = box[3];
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    std::vector<std::vector<double>> out(height , std::vector<double>(width , 0.0));
    for ( int y = 0 ; y < height; y++) {
        for (int x = 0; x < width; x++) {
            out[y][x] = luminance[minY + y][minX + x];
        }
    }
    return out;
}

void Ascii::generate(){
    std::cout << getPath() << std::endl;
    int n{};
    int width{};
    int height{};
    int desired_channels = 4;
    //Here x and y are the values for the array position and n is channels for the photo!
    unsigned char* pixel_data = stbi_load( image_path.c_str(),&width,&height,&n, desired_channels);

    if (!pixel_data) {
        std::cerr << "Failed to load image!" << stbi_failure_reason() << std::endl;
    }

    std::cout << "Width: " << width << " Height: " << height << " Channels: " << n << std::endl;
    std::vector<double> temp_array;
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
            double r_byte = pixel_data[index];
            double g_byte = pixel_data[index + 1];
            double b_byte = pixel_data[index + 2];
            double a_byte = pixel_data[index + 3];
            double alpha = a_byte / 255.0;
            rf = alpha * (r_byte/255.0) + (1.0 - alpha) * 1.0;
            gf = alpha * (g_byte/255.0) + (1.0 - alpha) * 1.0;
            bf = alpha * (b_byte/255.0) + (1.0 - alpha) * 1.0;
            //Have linearize for each value!!!! //Not Important but gets more sharp output :) (when sRGB)
            getLinearized(temp_array,rf,gf,bf);
            
            //Calculate Linear Luminance For That Pixel!!!
            linear_luminance = 0.2126 * temp_array[0] + 0.7152 * temp_array[1] + 0.0722 * temp_array[2];
            luminance[y][x] = linear_luminance;
        }
    }
    stbi_image_free(pixel_data);
    
    /*std::cout << "First Pixel luminance value: " << luminance[0][0] << std::endl;
    std::cout << "Last Pixel luminance value: " << luminance[height-1][width-1] << std::endl;
    */

    float char_aspect = getCharAspect();
    int cols = getWidth();
    int rows{};
    if (getHeight() == 0){
        rows = round(cols * (height / (double) width) * char_aspect);    
    } else {
        rows = getHeight();
    }
    
    std::cout << "Rows: " << rows << " Columns: " << cols << std::endl;

    std::vector<std::vector<double>> l_cell(rows , std::vector<double>(cols));

    double minV=1 , maxV = 0, sum=0; int count = 0;
    
    //Debugging Purpose!!
    for(int i = 0 ; i < height ; i++) for (int j = 0 ; j < width ; j++){
        double v = luminance[i][j];
        minV = std::min(minV , v);
        maxV = std::max(maxV , v);
        sum += v; count++;
    }
    /*
    std::cout << "Value of mean: " << (sum/count) << " Max: " << maxV << " Min: " << minV << std::endl;*/
    double meanY = sum / count;

    auto box = computeBoxForCrop(luminance, width, height, meanY);
    int cropped_width = box[2] - box[0] + 1;
    int cropped_height = box[3] - box[1] + 1;

    /*std::cout << "Debugs : " << box[0] << " " << box[1] << " " << box[2] << " " << box[3] << " " << std::endl;*/
    
    auto luminance_cropped = MainCrop(luminance, box);

    //Resampling the pixels to a block ( convert to a function !)
    getResampling(luminance_cropped, l_cell, cropped_width, cropped_height, cols, rows);

    /*std::cout << "First value of luminance for the block after resampling >> " << l_cell[0][0] << std::endl;*/



    /////////////////////////////////////////////////////////////////////////////// Here Brightness starts!!

    std::vector<std::vector<double>> added_values(rows , std::vector<double>(cols));
    
    //Add Brightness , Contrast , Gamma etc ..
    double brightness = getBrightness();
    double contrast   = getContrast();
    double gamma      = getGamma();
    
    for ( int i = 0 ; i < rows ; i++){
        for (int j = 0; j < cols ; j++){
            double value = l_cell[i][j];
            double v_contrast = (value - meanY) * contrast + meanY;
            double v_brightness = v_contrast + brightness;
            double v_clamp = std::clamp(v_brightness , 0.0 , 1.0);
            /*if (i == 4 && j == 4) {
                std::cout << "value=" << value 
                << " v_contrast=" << v_contrast
                << " v_brightness=" << v_brightness
                << " v_clamp=" << v_clamp
                << " gamma=" << gamma << '\n';
            }*/
            double v_value = std::pow(v_clamp , 1.0/gamma);
            added_values[i][j] = v_value;
        }
    }

    /*std::cout << "Check : " << added_values[4][4] << std::endl;*/
    
    std::string ramp = getCharLevel();
    int N_ramp = ramp.length();

    double val{};
    for ( int i = 0; i < rows ; i++){
        for (int j = 0; j < cols ; j++){
            val = added_values[i][j];
            int index = int(std::round(val * (N_ramp - 1)));
            index = std::clamp(index , 0 , N_ramp - 1);
            std::cout << ramp[index];
        }
        std::cout << "\n";
    }
}   