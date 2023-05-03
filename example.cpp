#include <pybind11/pybind11.h>
#include <vector>
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>

std::vector<double> Mandel(std::vector<double> z, std::vector<double> c) {
    double z_new_re = z[0]*z[0] - z[1]*z[1] + c[0];
    double z_new_im = 2*z[0]*z[1] + c[1];
    std::vector<double> z_new = {z_new_re,z_new_im};
    return z_new;
}

double mag(std::vector<double> z) {
    return z[0]*z[0] + z[1]*z[1];
}


std::vector<std::vector<int>> render_set() {
    int width = 100;
    int height = 100;

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
        int max_iter = 10000;
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


    executor.run(taskflow).wait();

    return pixel_data;
}

PYBIND11_MODULE(cppmandelbrot, m) {
    m.doc() = "pybind11 plugin for rendering the mandelbrot set"; // optional module docstring

    m.def("render_set", &render_set, "A function that renders the mandelbrot set");
}