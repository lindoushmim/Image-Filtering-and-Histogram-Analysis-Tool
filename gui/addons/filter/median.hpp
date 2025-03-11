#pragma once

struct Median : Mif02Plugin
{
	fn getName() const -> string_view override
	{
		return "Filtre median";
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
			cv::medianBlur(loadedImage, filteredImg, kernelSize);
		} else {
			filteredImg = applyFilterMedian(loadedImage, kernelSize);
		}
	}
};
REGISTER_PLUGIN(Median);
