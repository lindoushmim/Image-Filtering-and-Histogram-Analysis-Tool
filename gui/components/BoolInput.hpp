#pragma once

struct BoolInput
{
	wxCheckBox* checkbox = nullptr;

	string_view 	label;
	bool 			default_value;

	BoolInput(string_view label = "", bool default_value = "") : label(label), default_value(default_value)
	{
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel, std::optional<string_view> label_opt = std::nullopt, std::optional<bool> default_value_opt = std::nullopt) -> void
	{
		string_view label = label_opt.value_or(this->label);
		bool 		default_value = default_value_opt.value_or(this->default_value);

		checkbox = new wxCheckBox(panel, wxID_ANY, wxString(label.data(), label.size()));
		checkbox->SetValue(default_value);
		vbox->Add(checkbox, 0, wxALL, 10);
	}
};
