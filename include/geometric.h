#ifndef GEOMETRIC_H
# define GEOMETRIC_H
# include <opencv2/opencv.hpp>
# include <iostream>

// eveything about geometric transformations ....

cv::Mat applyResize(const cv::Mat& image, double scale);
cv::Mat	applyRotation(const cv::Mat&  loadedImage, double angle);

cv::Mat testResizeWithOpenCv(const cv::Mat& image, double scale);
cv::Mat	testRotationWithOpenCv(const cv::Mat&  loadedImage, double angle);


#endif
