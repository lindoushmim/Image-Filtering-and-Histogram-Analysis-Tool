#pragma once

struct HistogramShow : Mif02Plugin
{
	fn getName() const -> string_view override
	{
		return "Voir Histogram";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		useLogNormInHistComponent.setupUi(vbox, panel);
	}


	fn onApply(const cv::Mat& loadedImage, cv::Mat& filteredImg) -> void override
	{
		new CVImageWindow(nullptr, "Histograme", generateHistImage(filteredImg, useLogNormInHistComponent.checkbox->IsChecked()),  315, -330);
	}
};

REGISTER_PLUGIN(HistogramShow);
