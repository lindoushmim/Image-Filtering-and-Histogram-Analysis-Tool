#include "histogram.h"
#include <numeric>
#include <iostream>

int Round(float value)
{
	if (value > 0) {
		return (int)(value + 0.5f);
	} else {
		return (int)(value - 0.5f);
	}
}

template <typename T>
T saturate_cast(int value)
{
	if (value < 0) {
		return 0;
	}
	if (value > 255) {
		return 255;
	}
	return static_cast<T>(value);
}
template uchar saturate_cast<uchar>(int value);


histogram calcHist(const cv::Mat& img, int channel)
{
	histogram output = {0};

	if (img.channels() != 3) {
		throw std::runtime_error("L'image doit avoir trois canneaux (BGR)");
	}

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
			int value = pixel[channel];
			output[value]++;
		}
	}

	return output;
}

void normalizeHistLog(uint pixels, histogram &hist, uint histHeight)
{
	uint totalPixels = pixels;
	if (totalPixels == 0)
		return;
	for (int i = 0; i < hist.size(); i++) {
		float logValue = std::log(hist[i] + 1);
		float logTotal = std::log(totalPixels + 1);
		hist[i] = static_cast<int>((logValue / logTotal) * histHeight);
	}
}

void normalizeHist(uint pixels, histogram &hist, uint histHeight)
{
	int maxHistValue = *std::max_element(hist.begin(), hist.end());
	if (maxHistValue == 0) return;
	float scale = 256 / static_cast<float>(maxHistValue);
	for (int i = 0; i < hist.size(); i++) {
		hist[i] = Round(hist[i] * scale);
	}
}

cv::Mat generateHistImage(const cv::Mat& img, bool logNorm)
{
	auto bHist = calcHist(img, 0);  // B
	auto gHist = calcHist(img, 1);  // G
	auto rHist = calcHist(img, 2);  // R

	if (logNorm)
	{
		normalizeHistLog(img.cols * img.rows, bHist, 256);
		normalizeHistLog(img.cols * img.rows, gHist, 256);
		normalizeHistLog(img.cols * img.rows, rHist, 256);
	}
	else
	{
		normalizeHist(img.cols * img.rows, bHist, 256);
		normalizeHist(img.cols * img.rows, gHist, 256);
		normalizeHist(img.cols * img.rows, rHist, 256);
	}

	int histHeight = 256;
	int histWidth = 512;
	cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(0, 0, 0));

	int histSize = 256;
	int binWidth = histWidth / histSize;

	for (int i = 1; i < histSize; i++)
	{
		cv::line(histImage,
				 cv::Point((i - 1) * binWidth, histHeight - Round(bHist[i - 1])),
				 cv::Point(i * binWidth, histHeight - Round(bHist[i])),
				 cv::Scalar(255, 0, 0), 2, 8, 0);

		cv::line(histImage,
				 cv::Point((i - 1) * binWidth, histHeight - Round(gHist[i - 1])),
				 cv::Point(i * binWidth, histHeight - Round(gHist[i])),
				 cv::Scalar(0, 255, 0), 2, 8, 0);

		cv::line(histImage,
				 cv::Point((i - 1) * binWidth, histHeight - Round(rHist[i - 1])),
				 cv::Point(i * binWidth, histHeight - Round(rHist[i])),
				 cv::Scalar(0, 0, 255), 2, 8, 0);
	}

	for (int i = 0; i < histSize; i += 32)
	{
		std::string label = std::to_string(i);
		cv::putText(histImage, label, cv::Point(i * binWidth, histHeight - 10),
					cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1, 8, false);
	}

	return histImage;
}



cv::Mat equalizeHist(const cv::Mat& img, double contrast_factor)
{
	if (img.channels() != 3) {
		throw std::runtime_error("L'image doit avoir trois canals (BGR)");
	}

	cv::Mat result = img.clone();

	for (int c = 0; c < 3; c++) {
		histogram hist = calcHist(img, c);

		histogram cdf = {0};
		cdf[0] = hist[0];
		for (int i = 1; i < 256; i++) {
			cdf[i] = cdf[i - 1] + hist[i];
		}

		auto min_cdf = cdf[0];
		auto max_cdf = cdf[255];
		for (int i = 0; i < 256; i++) {
			cdf[i] = ((cdf[i] - min_cdf) * 255) / (max_cdf - min_cdf);
		}

		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
				int new_value = static_cast<int>(cdf[pixel[c]] * contrast_factor);
				new_value = std::min(std::max(new_value, 0), 255);
				result.at<cv::Vec3b>(y, x)[c] = static_cast<uchar>(new_value);
			}
		}
	}

	return result;
}


cv::Mat stretchHist(const cv::Mat& img, double stretch_factor)
{
	if (img.channels() != 3) {
		throw std::runtime_error("L'image doit avoir trois canaux (BGR)");
	}
	cv::Mat result = img.clone();
	for (int c = 0; c < 3; c++) {
		histogram hist = calcHist(img, c);
		int min_val = 255, max_val = 0;
		for (int i = 0; i < 256; i++) {
			if (hist[i] > 0) {
				min_val = std::min(min_val, i);
				max_val = std::max(max_val, i);
			}
		}
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
				int new_value = static_cast<int>((pixel[c] - min_val) * 255.0 / (max_val - min_val) * stretch_factor);
				new_value = std::min(std::max(new_value, 0), 255);
				result.at<cv::Vec3b>(y, x)[c] = static_cast<uchar>(new_value);
			}
		}
	}
	return result;
}

cv::Mat shiftColors(const cv::Mat& img, int shiftB, int shiftG, int shiftR) {
	cv::Mat result = img.clone();

	for (uint y = 0; y < img.rows; y++) {
		for (uint x = 0; x < img.cols; x++) {
			cv::Vec3b& pixel = result.at<cv::Vec3b>(y, x);

			pixel[0] = saturate_cast<uchar>(pixel[0] + shiftB); //B
			pixel[1] = saturate_cast<uchar>(pixel[1] + shiftG); // G
			pixel[2] = saturate_cast<uchar>(pixel[2] + shiftR); // R
		}
	}

	return result;
}



void applyLUTToChannel(cv::Mat& image, const lut& channelLUT, int channelIndex)
{
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; ++x) {
			cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
			pixel[channelIndex] = channelLUT[pixel[channelIndex]];
		}
	}
}

void applyLUTsToChannels(cv::Mat& image, const lut& rLUT, const lut& gLUT, const lut& bLUT)
{
	applyLUTToChannel(image, bLUT, 0);
	applyLUTToChannel(image, gLUT, 1);
	applyLUTToChannel(image, rLUT, 2);
}

lut createReducedLUT(uint levels) {
	lut table;
	uint step = 256 / levels;
	for (uint i = 0; i < 256; ++i) {
		table[i] = (i / step) * step;
	}
	return table;
}

cv::Mat compressImage(const cv::Mat& input, uint levels)
{
	lut rLUT = createReducedLUT(levels);
	lut gLUT = createReducedLUT(levels);
	lut bLUT = createReducedLUT(levels);

	auto image = input.clone();
	applyLUTsToChannels(image, rLUT, gLUT, bLUT);

	return image;
}


cv::Mat compressImageWithOpenCV(const cv::Mat& input, uint numColors)
{
	cv::Mat data;
	input.convertTo(data, CV_32F);
	data = data.reshape(1, input.rows * input.cols);

	cv::Mat labels, centers;
	cv::kmeans(data, numColors, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.2), 3, cv::KMEANS_RANDOM_CENTERS, centers);

	cv::Mat compressedImage(input.size(), input.type());
	for (uint i = 0; i < input.rows * input.cols; ++i) {
		uint label = labels.at<uint>(i);
		compressedImage.at<cv::Vec3b>(i / input.cols, i % input.cols) = cv::Vec3b(
			static_cast<uchar>(centers.at<float>(label, 0)),
			static_cast<uchar>(centers.at<float>(label, 1)),
			static_cast<uchar>(centers.at<float>(label, 2))
		);
	}

	return compressedImage;
}

lut calculateLutBetweenImages(const cv::Mat& image1, const cv::Mat& image2)
{
	lut lookupTable = {};

	if (image1.size() != image2.size() || image1.type() != image2.type())
		throw std::runtime_error("Les images doivent avoir la même taille et le même type.");

	for (uint i = 0; i < image1.rows; ++i) {
		for (uint j = 0; j < image1.cols; ++j) {
			uint val1 = image1.at<uchar>(i, j);
			uint val2 = image2.at<uchar>(i, j);
			lookupTable[val1] = val2;
		}
	}

	return lookupTable;
}

cv::Mat generateLUTImage(const lut& transformation)
{
	uint width = 512;
	uint height = 400;
	cv::Mat lutImage(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

	for (int i = 0; i <= 10; i++) {
		int y = height - i * (height / 10);
		std::string label = std::to_string(i * 25);
		cv::putText(
			lutImage,
			label,
			cv::Point(10, y + 2),
			cv::FONT_HERSHEY_SIMPLEX,
			0.5,
			cv::Scalar(255, 255, 255),
			1
		);
	}

	for (int i = 0; i <= 10; i++) {
		int x = i * (width / 10);
		std::string label = std::to_string(i * 25);
		cv::putText(
			lutImage,
			label,
			cv::Point(x, height - 10),
			cv::FONT_HERSHEY_SIMPLEX,
			0.5,
			cv::Scalar(255, 255, 255),
			1
		);
	}

	cv::line(lutImage, cv::Point(0, height), cv::Point(width, height), cv::Scalar(255, 255, 255), 2);
	cv::line(lutImage, cv::Point(0, height), cv::Point(0, 0), cv::Scalar(255, 255, 255), 2);

 	for (int i = 1; i < 256; i++) {
		cv::line(
			lutImage,
			cv::Point((i - 1) * 2, height - transformation[i - 1] * height / 256),
			cv::Point(i * 2, height - transformation[i] * height / 256),
			cv::Scalar(255, 255, 255),
			2
		);
	}

	return lutImage;
}
