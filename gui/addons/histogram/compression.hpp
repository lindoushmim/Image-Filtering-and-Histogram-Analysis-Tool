#pragma once

struct Compression : Mif02Plugin
{
	Components::TextInput			factorComponent;

	fn getName() const -> string_view override
	{
		return "Compression simple (reduction de couleur)";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		factorComponent.setupUi(vbox, panel, "Facteur :", "1");
		useOpenCVComponent.setupUi(vbox, panel);
		useLogNormInHistComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat& filteredImg) -> void override
	{
		double factor = stod(factorComponent.ctrl->GetValue().ToStdString());

		new CVImageWindow(nullptr, "Histograme avant traitement", generateHistImage(loadedImage, useLogNormInHistComponent.checkbox->IsChecked()),  315, -330);

		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = compressImageWithOpenCV(loadedImage, factor);
		} else {
			filteredImg = compressImage(loadedImage, factor);
		}
		new CVImageWindow(nullptr, "Histograme apres traitement", generateHistImage(filteredImg, useLogNormInHistComponent.checkbox->IsChecked()), 315, -15);

		new CVImageWindow(nullptr, "LUT", generateLUTImage(calculateLutBetweenImages(loadedImage, filteredImg)));
	}
};

REGISTER_PLUGIN(Compression);
