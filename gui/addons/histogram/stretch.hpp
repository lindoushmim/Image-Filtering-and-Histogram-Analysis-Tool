#pragma once

struct HistogramStretch : Mif02Plugin
{
	Components::TextInput			factorComponent;

	fn getName() const -> string_view override
	{
		return "Etirer Histogram";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		factorComponent.setupUi(vbox, panel, "Facteur :", "1");
		useLogNormInHistComponent.setupUi(vbox, panel);
	}


	fn onApply(const cv::Mat& loadedImage, cv::Mat& filteredImg) -> void override
	{
		double factor = stod(factorComponent.ctrl->GetValue().ToStdString());
		new CVImageWindow(nullptr, "Histograme avant traitement", generateHistImage(loadedImage, useLogNormInHistComponent.checkbox->IsChecked()),  315, -330);
		filteredImg = equalizeHist(loadedImage, factor);
		new CVImageWindow(nullptr, "Histograme apres traitement", generateHistImage(filteredImg, useLogNormInHistComponent.checkbox->IsChecked()), 315, -15);
	}
};

REGISTER_PLUGIN(HistogramStretch);
