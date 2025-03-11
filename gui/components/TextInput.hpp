#pragma once

struct TextInput
{
	wxTextCtrl* ctrl = nullptr;


	string_view label;
	string_view default_text;

	TextInput(string_view label = "", string_view default_text = "") : label(label), default_text(default_text)
	{
	}

	fn setupUi(wxBoxSizer* vbox, wxPanel* panel, std::optional<string_view> label_opt = std::nullopt, std::optional<string_view> default_text_opt = std::nullopt) -> void
	{
		string_view label = label_opt.value_or(this->label);
		string_view default_text = default_text_opt.value_or(this->default_text);

		wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText*  paramLabel = new wxStaticText(panel, wxID_ANY, wxString(label.data(), label.size()));
		ctrl = new wxTextCtrl(panel, wxID_ANY, wxString(default_text.data(), default_text.size()));
		hbox3->Add(paramLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
		hbox3->Add(ctrl, 1, wxEXPAND);
		vbox->Add(hbox3, 0, wxEXPAND | wxALL, 10);
	}
};
