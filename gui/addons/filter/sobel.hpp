#pragma once

struct Sobel : Mif02Plugin
{
	Components::TextInput			directionComponent;

	fn getName() const -> string_view override
	{
		return "Sobel";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		kernelSizeComponent.setupUi(vbox, panel);
		directionComponent.setupUi(vbox, panel, "Direction (0 vert, 1 horizontal) :", "0");
		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{
		uint kernelSize =kernelSizeComponent.slider->GetValue();
		uint choiceDirection = parse_bool(directionComponent.ctrl->GetValue().ToStdString());


		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = testSobelKernelWithOpenCv(loadedImage,kernelSize,choiceDirection);
		} else {
			filteredImg = applyConvolution(loadedImage, createSobelKernel(kernelSize, choiceDirection));
		}
	}
};
REGISTER_PLUGIN(Sobel);
