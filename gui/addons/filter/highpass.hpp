#pragma once

struct Highpass : Mif02Plugin
{
	fn getName() const -> string_view override
	{
		return "Filtre passe haut";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		kernelSizeComponent.setupUi(vbox, panel);
		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{
		uint kernelSize =kernelSizeComponent.slider->GetValue();

		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = testHighPassKernelWithOpenCv(loadedImage, kernelSize);
		} else {
			filteredImg = applyConvolution(loadedImage,createHighPassKernel(kernelSize));
		}
	}
};
REGISTER_PLUGIN(Highpass);
