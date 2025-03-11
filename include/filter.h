#ifndef FILTER_H
#define FILTER_H

#include <opencv2/opencv.hpp>
#include <iostream>



////////////////////////////// file for everything that is filtering //////////////////////////////



// return an image from another where a median filter has been applied
cv::Mat medianFilterGray(const cv::Mat& image, int kernelSize); 
cv::Mat medianFilterColor(const cv::Mat& image, int kernelSize); 
cv::Mat applyFilterMedian(const cv::Mat& img, int kernelSize); 

// return an image from another where a convolotion has been applied

cv::Mat genericConvolutionGray(const cv::Mat& image, const cv::Mat& kernel);
cv::Mat genericConvolutionColor(const cv::Mat& image, const cv::Mat& kernel);
cv::Mat applyConvolution(const cv::Mat & img, const cv::Mat& kernel); 
cv::Mat createAveragingKernel(int size); 
cv::Mat createGaussianKernel(int size, float sigma); 
cv::Mat createLaplacianKernel(int size); 
cv::Mat createSobelKernel(int size, bool horizontal); 
cv::Mat createHighPassKernel(int size); 

// comparaison convolution with open cv 

cv::Mat testAveragingKernelWithOpenCv(const cv::Mat &img, int size); 
cv::Mat testGaussianKernelWithOpenCv(const cv::Mat &img, int sizeKernel, double alpha); 
cv::Mat testSobelKernelWithOpenCv(const cv::Mat &img, int choiceSizeKernel, int choiceDirection); 
cv::Mat testHighPassKernelWithOpenCv(const cv::Mat &img, int choice); 
cv::Mat testLaplacianKernelWithOpenCv(const cv::Mat &img, int kernelSize); 


#endif