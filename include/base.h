#ifndef BASE_H
#define BASE_H 

#include <opencv2/opencv.hpp>
#include <iostream>



/////////////////////    file for utility functions => loading images, entering parameters, displaying


// to load a image from specific file
cv::Mat loadImage(const std::string& filePath); 

// to display a image in a window
void showImage(const cv::Mat& img, const std::string& window); 


#endif