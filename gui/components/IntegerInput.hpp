#include <wx/wx.h>
#include <string_view>

using namespace std;

struct IntegerInput {
	wxSlider* slider = nullptr;

	string_view label;
	int default_value;
	int min_value;
	int max_value;

	IntegerInput(string_view label = "", int default_value = 0, int min_value = 0, int max_value = 0)
		: label(label), default_value(default_value), min_value(min_value), max_value(max_value) {

		if (default_value % 2 == 0) {
			default_value += 1;
		}
	}

	void setupUi(
					wxBoxSizer* vbox,
					wxPanel* panel,
					std::optional<string_view> label_opt = std::nullopt,
					std::optional<int> default_value_opt = std::nullopt,
					std::optional<int> min_value_opt = std::nullopt,
					std::optional<int> max_value_opt = std::nullopt)
	{
		string_view label = label_opt.value_or(this->label);
		int default_value = default_value_opt.value_or(this->default_value);
		int min_value = min_value_opt.value_or(this->min_value);
		int max_value = max_value_opt.value_or(this->max_value);

		wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

		wxStaticText* paramLabel = new wxStaticText(panel, wxID_ANY, wxString(label.data(), label.size()));
		hbox->Add(paramLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

		slider = new wxSlider(panel, wxID_ANY, default_value, min_value, max_value, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
		hbox->Add(slider, 1, wxEXPAND);
		vbox->Add(hbox, 0, wxEXPAND | wxALL, 10);
	}

	fn getValue() -> int
	{
		return slider->GetValue();
	}
};
