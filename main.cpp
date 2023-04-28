#include "bitmap.hpp"
#include <vector>
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>



void generatePixelData(std::vector<uint8_t>& pixel_data, int width, int height, int start_row, int end_row) {
    // Loop through the rows assigned to this task
    for (int y = start_row; y < end_row; ++y) {
        for (int x = 0; x < width; ++x) {
            // Generate RGB values for each pixel
            uint8_t red = static_cast<uint8_t>(x % 256);
            uint8_t green = static_cast<uint8_t>(y % 256);
            uint8_t blue = static_cast<uint8_t>((x + y) % 256);

            // Calculate the pixel index in the pixel_data vector
            size_t pixel_index = (y * width + x) * 3;

            // Set the RGB values in the pixel_data vector
            pixel_data[pixel_index] = blue;
            pixel_data[pixel_index + 1] = green;
            pixel_data[pixel_index + 2] = red;
        }
    }
}

template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> const &vec)
{
    std::vector<T> flattened;
    for (auto const &v: vec) {
        flattened.insert(flattened.end(), v.begin(), v.end());
    }
    return flattened;
}

std::vector<double> Mandel(std::vector<double> z, std::vector<double> c) {
    double z_new_re = z[0]*z[0] - z[1]*z[1] + c[0];
    double z_new_im = 2*z[0]*z[1] + c[1];
    std::vector<double> z_new = {z_new_re,z_new_im};
    return z_new;
}

double mag(std::vector<double> z) {
    return z[0]*z[0] + z[1]*z[1];
}


int main() {
    int width = 3000;
    int height = 3000;

    std::vector<int> pixel = {0,0,0};
    std::vector<std::vector<int>> pixel_data(width * height, pixel); 

    tf::Taskflow taskflow;
    tf::Executor executor;

    auto generate_task = taskflow.for_each_index(0, width*height,  1, 
    [&](int i)
    { 
        int x = i%width;
        int y = i/width;
        std::vector<double> offset = {-0.3484,-0.60641};
        double scale = 0.00000001;
        std::vector<double> c = {scale*(x-width/2)+offset[0],scale*(y-height/2)+offset[1]};

        std::vector<double> z = {0,0};
        std::vector<int> pixel_color;

        int iter = 0;
        int max_iter = 1000;
        while(iter < max_iter && mag(z) < 1000000000000000000) {
            z = Mandel(z,c);

            iter++;
        }

        //std::cout << iter << std::endl;

        if (iter == max_iter) {
            pixel_color = {256,256,256};
        } else {
            pixel_color = {1*iter,1*iter,1*iter};
        }
        //std::cout << pixel_color[0] << std::endl;
        pixel_data[i] = (pixel_color);  
    });

    auto write_task = taskflow.emplace([&]() 
    {
        std::vector<int> flattened_pixels = flatten(pixel_data);
        std::vector<uint8_t> raw_data;

        for (auto i: flattened_pixels) {
            raw_data.push_back(static_cast<uint8_t>(i));
        }

        generateBitmap("image.bmp", width, height, raw_data);



    });

    generate_task.precede(write_task);

    executor.run(taskflow).wait();

    return 0;
}


