#pragma once

struct Laplacian : Mif02Plugin
{
	fn getName() const -> string_view override
	{
		return "Laplacien";
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
			filteredImg = testLaplacianKernelWithOpenCv(loadedImage, kernelSize);
		} else {
			filteredImg = applyConvolution(loadedImage,createLaplacianKernel(kernelSize));
		}
	}
};
REGISTER_PLUGIN(Laplacian);
