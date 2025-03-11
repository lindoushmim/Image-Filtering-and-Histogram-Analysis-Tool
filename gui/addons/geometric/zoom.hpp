#pragma once

struct Zoom : Mif02Plugin
{
	Components::TextInput			scaleComponent;

	fn getName() const -> string_view override
	{
		return "Zoom";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		scaleComponent.setupUi(vbox, panel, "Facteur d'échelle :", "1.0");
		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{
		double scale = stod(scaleComponent.ctrl->GetValue().ToStdString());

		if (scale <= 0.0) {
			throw invalid_argument("Le facteur d'échelle doit être strictement positif.");
		}

		cv::Mat resized;

		if (useOpenCVComponent.checkbox->IsChecked()) {
			resized = testResizeWithOpenCv(loadedImage, scale);
		} else {
			resized = applyResize(loadedImage, scale);
		}

 		int offsetX = (resized.cols - loadedImage.cols) / 2;
		int offsetY = (resized.rows - loadedImage.rows) / 2;

		cv::Rect roi(
			max(0, offsetX),
			max(0, offsetY),
			min(loadedImage.cols, resized.cols),
			min(loadedImage.rows, resized.rows)
		);

		filteredImg = resized(roi);
	}

};
REGISTER_PLUGIN(Zoom);
