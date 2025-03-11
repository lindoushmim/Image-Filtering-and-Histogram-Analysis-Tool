#include "geometric.h"
#include <cmath>

cv::Mat applyResize(const cv::Mat& image, double scale)
{
	cv::Mat resized;
	int newWidth = static_cast<int>(image.cols * scale);
	int newHeight = static_cast<int>(image.rows * scale);

	resized.create(newHeight, newWidth, image.type());

	for (int y = 0; y < newHeight; ++y) {
		for (int x = 0; x < newWidth; ++x) {
			int srcX = std::min(static_cast<int>(x / scale), image.cols - 1);
			int srcY = std::min(static_cast<int>(y / scale), image.rows - 1);
			resized.at<cv::Vec3b>(y, x) = image.at<cv::Vec3b>(srcY, srcX);
		}
	}

	return resized;
}


cv::Mat	testResizeWithOpenCv(const cv::Mat &image, double scale) {
	cv::Mat resized;
	cv::resize(image, resized, cv::Size(), scale, scale, cv::INTER_LINEAR);
	return resized;
}


cv::Mat	applyRotation(const cv::Mat&  loadedImage, double angle)
{
	double radians = -angle * CV_PI / 180.0;
	double cosTheta = cos(radians);
	double sinTheta = sin(radians);

	int newWidth = static_cast<int>(loadedImage.cols * std::abs(cosTheta) + loadedImage.rows * std::abs(sinTheta));
	int newHeight = static_cast<int>(loadedImage.cols * std::abs(sinTheta) + loadedImage.rows * std::abs(cosTheta));

	cv::Mat rotatedImg(newHeight, newWidth, loadedImage.type(), cv::Scalar(0, 0, 0));

	int centerX = newWidth / 2;
	int centerY = newHeight / 2;

	for (int y = 0; y < rotatedImg.rows; y++) {
		for (int x = 0; x < rotatedImg.cols; x++) {
			// Calculate the original coordinates after rotation
			int srcX = static_cast<int>((x - centerX) * cosTheta + (y - centerY) * sinTheta + loadedImage.cols / 2.0);
			int srcY = static_cast<int>(-(x - centerX) * sinTheta + (y - centerY) * cosTheta + loadedImage.rows / 2.0);

			// Check if the source coordinates are within the bounds of the loaded image
			if (srcX >= 0 && srcX < loadedImage.cols && srcY >= 0 && srcY < loadedImage.rows) {
				rotatedImg.at<cv::Vec3b>(y, x) = loadedImage.at<cv::Vec3b>(srcY, srcX);
			}
		}
	}

	return rotatedImg;
}


cv::Mat	testRotationWithOpenCv(const cv::Mat&  loadedImage, double angle) {

	cv::Mat filteredImg;

	cv::Point2f center(loadedImage.cols / 2.0, loadedImage.rows / 2.0);

	cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), loadedImage.size(), angle).boundingRect2f();
	rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;
	cv::warpAffine(loadedImage, filteredImg, rotationMatrix, bbox.size());

	return filteredImg;
}

