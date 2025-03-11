#include "filter.h"


///////////////////////////////////////// MEDIAN FILTER /////////////////////////////////////////


 cv::Mat medianFilterGray(const cv::Mat& image, int kernelSize) {
	if (image.channels() != 1) {
		std::cerr << "Only gray images can be treated." << std::endl;
		return cv::Mat();
	}

	if (kernelSize % 2 == 0 || kernelSize <= 1) {
		std::cerr << "Kernel size must be an odd number greater than 1!" << std::endl;
		return cv::Mat();
	}

	int halfKernel = kernelSize / 2;
	cv::Mat result = image.clone();

	for (int i = halfKernel; i < image.rows - halfKernel; ++i) {
		for (int j = halfKernel; j < image.cols - halfKernel; ++j) {
			std::vector<uchar> pixels; // for storing the values of the neighbours pixels
			for (int y = -halfKernel; y <= halfKernel; ++y) {
				for (int x = -halfKernel; x <= halfKernel; ++x) {
					pixels.push_back(image.at<uchar>(i + y, j + x));
				}
			}

			std::sort(pixels.begin(), pixels.end());
			uchar medianValue = pixels[pixels.size() / 2];
			result.at<uchar>(i, j) = medianValue;
		}
	}

	return result;
}


cv::Mat medianFilterColor(const cv::Mat& image, int kernelSize) {
	if (image.channels() != 3) {
		std::cerr << "Only color images can be treated!" << std::endl;
		return cv::Mat();
	}

	if (kernelSize % 2 == 0 || kernelSize <= 1) {
		std::cerr << "Kernel size must be an odd number greater than 1!" << std::endl;
		return cv::Mat();
	}

	std::vector<cv::Mat> channels;
	cv::split(image, channels);

	for (int i = 0; i < 3; ++i) {
		channels[i] = medianFilterGray(channels[i], kernelSize);
	}

	cv::Mat filteredImage;
	cv::merge(channels, filteredImage);
	return filteredImage;
}

cv::Mat applyFilterMedian(const cv::Mat& img, int kernelSize) {
	return img.channels() == 1 ? medianFilterGray(img, kernelSize) : medianFilterColor(img, kernelSize);
}


///////////////////////////////////////// CONVOLUTION FILTER /////////////////////////////////////////


cv::Mat genericConvolutionGray(const cv::Mat &image, const cv::Mat &kernel) {
	if (image.channels() != 1) {
		std::cerr << "Only grayscale images are supported in genericConvolutionGray." << std::endl;
		return cv::Mat();
	}

	if (kernel.rows % 2 == 0 || kernel.cols % 2 == 0) {
		std::cerr << "The size of the kernel must be odd." << std::endl;
		return cv::Mat();
	}

	cv::Mat resultConvolution(image.rows, image.cols, image.type());

	const int dx = kernel.cols / 2;
	const int dy = kernel.rows / 2;

	int borderHandling = 0;

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			float sum = 0.0f;
			bool ignorePixel = false;

			for (int k = 0; k < kernel.rows; k++) {
				for (int l = 0; l < kernel.cols; l++) {
					int x = j - dx + l;
					int y = i - dy + k;

					switch (borderHandling) {
						case 0: // duplicate the pixel by the value of the near pixel neighoubrs
							x = std::max(0, std::min(x, image.cols - 1));
							y = std::max(0, std::min(y, image.rows - 1));
							break;
						case 1: // set the pixel to 0 => black padding
							if (x < 0 || x >= image.cols || y < 0 || y >= image.rows)
								continue;
							break;
						case 2: // set the pixel to 255 => white padding
							if (x < 0 || x >= image.cols || y < 0 || y >= image.rows) {
								sum += 255 * kernel.at<float>(k, l);
								continue;
							}
							break;
						case 3: // ignore the edges
							if (x < 0 || x >= image.cols || y < 0 || y >= image.rows) {
								ignorePixel = true;
							}
							break;
					}

					if (!ignorePixel) {
						sum += image.at<uchar>(y, x) * kernel.at<float>(k, l);
					}
				}
			}

			if (!ignorePixel) {
				resultConvolution.at<uchar>(i, j) = cv::saturate_cast<uchar>(sum);
			}
		}
	}

	return resultConvolution;
}




cv::Mat genericConvolutionColor(const cv::Mat& image, const cv::Mat& kernel) {
	if (image.channels() != 3) { // function only for color images
		std::cerr << "Only color images are supported for in genericConvolutionColor" << std::endl;
		return cv::Mat();
	}

	std::vector<cv::Mat> channels; // we get the three channels of the color image
	cv::split(image, channels); // extraction of the channels RGB

	for (unsigned int i = 0; i < 3; ++i) { // we calculate the convolution of the three channels separately
		channels[i] = genericConvolutionGray(channels[i], kernel);
	}

	cv::Mat filteredImage;
	cv::merge(channels, filteredImage); // we merge the channels into a single image
	return filteredImage;
}

cv::Mat applyConvolution(const cv::Mat & img, const cv::Mat& kernel) {
	return img.channels()==1 ? genericConvolutionGray(img,kernel) : genericConvolutionColor(img,kernel) ;
}



/////////// creation of the different kernel


cv::Mat createAveragingKernel(int size) {
	return cv::Mat::ones(size, size, CV_32F) / (float)(size * size);
}


cv::Mat createGaussianKernel(int size, float sigma) {
	int half_size = size / 2;
	cv::Mat kernel(size, size, CV_32F);

	float sum = 0.0f;

	for (int i = -half_size; i <= half_size; ++i) {
		for (int j = -half_size; j <= half_size; ++j) {
			float value = exp(-(i*i + j*j) / (2 * sigma * sigma)); // for each pixel we calculate the value of the gaussienne
			kernel.at<float>(i + half_size, j + half_size) = value;

			sum += value;
		}
	}
	kernel /= sum;
	return kernel;
}


cv::Mat createLaplacianKernel(int size) { // problem here
	int half_size = size / 2;
	cv::Mat kernel(size, size, CV_32F, cv::Scalar(0.0f));

	for (int i = -half_size; i <= half_size; ++i) {
		for (int j = -half_size; j <= half_size; ++j) {
			if (i == 0 && j == 0) { // center of the kernel
				kernel.at<float>(i + half_size, j + half_size) = -4.0f;
			}
			else if (i == 0 || j == 0) { // center neighbours
				kernel.at<float>(i + half_size, j + half_size) = 1.0f;
			}
			else { // corner
				kernel.at<float>(i + half_size, j + half_size) = 0.0f;
			}
		}
	}

	return kernel;
}


cv::Mat createSobelKernel(int size, bool horizontal) {
	int halfSize = size / 2;
	cv::Mat kernel(size, size, CV_32F, cv::Scalar(0));

	for (int i = -halfSize; i <= halfSize; ++i) {
		for (int j = -halfSize; j <= halfSize; ++j) {
			if (horizontal) {
				kernel.at<float>(i + halfSize, j + halfSize) = j;
			} else {
				kernel.at<float>(i + halfSize, j + halfSize) = i;
			}
		}
	}

	return kernel;
}

cv::Mat createHighPassKernel(int size) {
	cv::Mat kernel(size, size, CV_32F, cv::Scalar(-1));
	int center = size / 2;
	kernel.at<float>(center, center) = size * size - 1;

	return kernel;
}


//////////// comparaison with open cv

cv::Mat testAveragingKernelWithOpenCv(const cv::Mat &img, int size) {
	cv::Mat kernel = cv::Mat::ones(size, size, CV_32F) / (float)(size * size);
	cv::Mat result;
	cv::filter2D(img, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	return result;
}

cv::Mat testGaussianKernelWithOpenCv(const cv::Mat &img, int sizeKernel, double alpha) {
	cv::Mat imgConvOpenCV;
	cv::GaussianBlur(img, imgConvOpenCV, cv::Size(sizeKernel, sizeKernel), alpha, alpha);
	return imgConvOpenCV;
}

cv::Mat testSobelKernelWithOpenCv(const cv::Mat &img, int choiceSizeKernel, int choiceDirection) {
	cv::Mat imgConvOpenCV;
	cv::Sobel(img, imgConvOpenCV, CV_8U,
			  choiceDirection == 1 ? 1 : 0,
			  choiceDirection == 0 ? 1 : 0,
			  choiceSizeKernel);

	return imgConvOpenCV;
}

cv::Mat testHighPassKernelWithOpenCv(const cv::Mat &img, int choice) {
	cv::Mat blurred, highPassOpenCV;
	cv::GaussianBlur(img, blurred, cv::Size(choice, choice), 0);
	highPassOpenCV = img - blurred;
	return highPassOpenCV;
}

cv::Mat testLaplacianKernelWithOpenCv(const cv::Mat &img, int kernelSize) {
	cv::Mat laplacianImage, result;
	cv::Laplacian(img, laplacianImage, CV_16S, kernelSize);
	cv::convertScaleAbs(laplacianImage, result);
	return result;
}

