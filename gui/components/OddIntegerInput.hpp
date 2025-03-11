#include <wx/wx.h>
#include <string_view>

using namespace std;

struct OddIntegerInput : IntegerInput {

	using IntegerInput::IntegerInput;

	template <typename... Args>
	void setupUi(wxBoxSizer* vbox, wxPanel* panel, Args&&... args)
	{
		IntegerInput::setupUi(vbox, panel, std::forward<Args>(args)...);
		slider->Bind(wxEVT_SLIDER, &OddIntegerInput::onSliderChange, this);
	}

	void onSliderChange(wxCommandEvent& event) {
		int value = slider->GetValue();
		if (value % 2 == 0) {
			value += (value < max_value) ? 1 : -1;
			slider->SetValue(value);
		}
	}
};
