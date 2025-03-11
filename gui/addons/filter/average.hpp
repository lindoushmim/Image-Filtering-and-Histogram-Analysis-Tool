#pragma once

struct Average : Mif02Plugin
{
	fn getName() const -> string_view override
	{
		return "Filtre moyenneur";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		kernelSizeComponent.setupUi(vbox, panel);
		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{
		uint kernelSize = kernelSizeComponent.slider->GetValue();

		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = testAveragingKernelWithOpenCv(loadedImage, kernelSize);
		} else {
			filteredImg = applyConvolution(loadedImage,createAveragingKernel(kernelSize));
		}
	}
};
REGISTER_PLUGIN(Average);
