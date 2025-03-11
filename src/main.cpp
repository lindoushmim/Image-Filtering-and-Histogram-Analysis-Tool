#include "base.h"
#include "filter.h"
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

// to store teacher test images
const std::vector<std::pair<std::string, std::string>> imageStoring = {
	{"../data/data1/Autre.png", "Autre"},
	{"../data/data1/lena.png", "Lena"},
	{"../data/data1/lena2.png", "Lena 2"},
	{"../data/data1/lena3.png", "Lena 3"}
};


void testFiltreMedian(cv::Mat & img) {
	int choice = 0;
	std::cout<<"Wich size for ur avecraging kernel do u yant"<<std::endl;
	std::cin>>choice;
	cv::Mat imgMedianFilter = applyFilterMedian(img, choice);
	showImage(imgMedianFilter, "with Median Filter");
}

void testAveragingKernel(cv::Mat & img) {
	int choice = 0;
	std::cout<<"Wich size for ur avecraging kernel do u yant"<<std::endl;
	std::cin>>choice;

	cv::Mat kernel = createAveragingKernel(choice);
	cv::Mat imResult = applyConvolution(img,kernel);
	showImage(imResult, "with Averaging Kernel");

	cv::Mat imResultOpenCv = testAveragingKernelWithOpenCv(img,choice);
	showImage(imResult, "with Averaging Kernel With OpenCv");
}

void testGaussianKernel(cv::Mat & img) {
	int choiceSizeKernel = 0;
	double choiceAlpha = 0;
	std::cout<<"Wich size for ur gaussian kernel do u yant"<<std::endl;
	std::cin>>choiceSizeKernel;
	std::cout<<"Wich alpha do u yant"<<std::endl;
	std::cin>>choiceAlpha;

	cv::Mat imgResult = applyConvolution(img,createGaussianKernel(choiceSizeKernel,choiceAlpha));
	showImage(imgResult, "Gaussian Filter");

	cv::Mat imgConvOpenCV = testGaussianKernelWithOpenCv(img, choiceSizeKernel, choiceAlpha);
	showImage(imgConvOpenCV, "Gaussian Filter OpenCV)");
}

void testLaplacienKernel(cv::Mat & img) {
	int choiceSizeKernel = 0;
	std::cout<<"Wich size for ur laplacien kernel do u yant"<<std::endl;
	std::cin>>choiceSizeKernel;
	cv::Mat imgResult = applyConvolution(img,createLaplacianKernel(choiceSizeKernel));
	showImage(imgResult, "Laplacien Filter");

	cv::Mat laplacianImage = testLaplacianKernelWithOpenCv(img, choiceSizeKernel);
	showImage(laplacianImage, "Laplacian Filter OpenCv");

}


void testSobelKernel(cv::Mat & img) {
	int choiceSizeKernel = 0;
	bool choiceDirection = false; // false : vertical and true : horizontal
	std::cout << "Which size for your Sobel kernel do u want (msut be odd number)?" << std::endl;
	std::cin >> choiceSizeKernel;
	std::cout << "Which Sobel direction do you want => 0 for vertical and 1 for horizontal)?" << std::endl;
	std::cin >> choiceDirection;

	cv::Mat imgResult = applyConvolution(img, createSobelKernel(choiceSizeKernel, choiceDirection));
	showImage(imgResult, "With Sobel Filter");

	cv::Mat imgConvOpenCV = testSobelKernelWithOpenCv(img, choiceSizeKernel, choiceDirection);
	showImage(imgConvOpenCV, "Sobel with OpenCV");
}

void testHighPassKernel(cv::Mat & img) {
	int choice = 0;
	std::cout<<"Wich size for ur hight pass kernel do u yant"<<std::endl;
	std::cin>>choice;
	cv::Mat kernel = createHighPassKernel(choice);
	cv::Mat imResult = applyConvolution(img,kernel);
	showImage(imResult, "High Pass Filter");

	cv::Mat highPassOpenCV = testHighPassKernelWithOpenCv(img, choice);
	showImage(highPassOpenCV, "High-Pass Filter OpenCV");
}


int main() {
	std::cout << "11111" << std::endl;

	std::string imagePath = "../data/data1/photographe.png";
	cv::Mat img = loadImage(imagePath);
	showImage(img, "Normal Image");

	//testFiltreMedian(img);
	//testAveragingKernel(img);
	//testGaussianKernel(img);
	testLaplacienKernel(img);
	//testSobelKernel(img);
	//testHighPassKernel(img);

	return 0;
}
