#pragma once

struct Rotate : Mif02Plugin
{
	Components::TextInput			degreeComponent;

	fn getName() const -> string_view override
	{
		return "Rotate";
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel) -> void override
	{
		degreeComponent.setupUi(vbox, panel, "Angle de rotation (°) :", "0");
		useOpenCVComponent.setupUi(vbox, panel);
	}

	fn onApply(const cv::Mat& loadedImage, cv::Mat &filteredImg) -> void override
	{
		double angle = stod(degreeComponent.ctrl->GetValue().ToStdString());

		if (useOpenCVComponent.checkbox->IsChecked()) {
			filteredImg = testRotationWithOpenCv(loadedImage, angle);
		} else {
			filteredImg = applyRotation(loadedImage, angle);

		}
	}
};
REGISTER_PLUGIN(Rotate);
