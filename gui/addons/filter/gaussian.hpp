#pragma once

struct Gaussian : Mif02Plugin
{
	Components::TextInput			alphaComponent;

	fn getName() const -> string_view override
	{
		return "Flou gaussien";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		kernelSizeComponent.setupUi(vbox, panel);
		alphaComponent.setupUi(vbox, panel, "Moyenne :", "0.5");
 		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{

		uint kernelSize =kernelSizeComponent.slider->GetValue();
		double alpha = stod(alphaComponent.ctrl->GetValue().ToStdString());


		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = testGaussianKernelWithOpenCv(loadedImage,kernelSize, alpha);
		} else {
			filteredImg = applyConvolution(loadedImage,createGaussianKernel(kernelSize,alpha));
		}
	}
};
REGISTER_PLUGIN(Gaussian);
