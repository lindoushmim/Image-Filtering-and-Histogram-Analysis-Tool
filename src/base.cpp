#include "base.h"


cv::Mat loadImage(const std::string & filePath) {
	cv::Mat image = cv::imread(filePath);
	if(image.empty()) {
		std::cerr<<"Image cannot load"<<filePath<<std::endl;
	}
	return image;
}

void showImage(const cv::Mat &img, const std::string &window) {
	if(img.empty()) {
		std::cerr<<"Image cannot be display"<<std::endl;
		return;
	}
	cv::imshow(window, img);
	cv::waitKey(0);
}

