#pragma once

struct HistogramShift : Mif02Plugin
{
	Components::IntegerInput			redComponent;
	Components::IntegerInput			greenComponent;
	Components::IntegerInput			blueComponent;

	fn getName() const -> string_view override
	{
		return "Decaler Histogram";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		redComponent.setupUi(vbox, panel, "Décalage du rouge:", 0, -255, 255);
		greenComponent.setupUi(vbox, panel, "Décalage du vert:", 0, -255, 255);
		blueComponent.setupUi(vbox, panel, "Décalage du bleu:", 0, -255, 255);
		useLogNormInHistComponent.setupUi(vbox, panel, "Utiliser normalisation logarithmique dans l'histogramme", true);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat& filteredImg) -> void override
	{
		new CVImageWindow(nullptr, "Histograme avant traitement", generateHistImage(loadedImage, useLogNormInHistComponent.checkbox->IsChecked()),  315, -330);

		filteredImg = shiftColors(loadedImage, redComponent.getValue(), greenComponent.getValue(), blueComponent.getValue());

		new CVImageWindow(nullptr, "Histograme apres traitement", generateHistImage(filteredImg, useLogNormInHistComponent.checkbox->IsChecked()), 315, -15);

		new CVImageWindow(nullptr, "LUT", generateLUTImage(calculateLutBetweenImages(loadedImage, filteredImg)));

	}
};

REGISTER_PLUGIN(HistogramShift);
