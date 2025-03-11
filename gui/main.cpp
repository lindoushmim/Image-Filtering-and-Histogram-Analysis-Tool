#include <wx/wx.h>
#include <wx/utils.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <opencv2/opencv.hpp>
#include <wx/dcbuffer.h>

#include <charconv>
#include <string_view>
#include <memory>
#include <utility>
#include <format>
#define fn auto

using namespace std;

#include "base.h"

#include "filter.h"
#include "geometric.h"
#include "histogram.h"


/*
 * Class-outil pour remplacer cv::imshow afin de pouvoir fermer les previews.
 * Prend en param l'image a afficher et loffset pour decaler la window par rapport au centre de l'ecran.
 */
struct CVImageWindow : wxFrame
{
  CVImageWindow(wxWindow* parent, const wxString& title, const cv::Mat& img, int offsetX = 0, int offsetY = 0)
		: wxFrame(parent, wxID_ANY, title)
	{
		wxSize screenSize = wxGetDisplaySize();
		int screenCenterX = screenSize.GetWidth() / 2;
		int screenCenterY = screenSize.GetHeight() / 2;

		wxSize windowSize = this->GetSize();

		int windowPosX = screenCenterX - windowSize.GetWidth() / 2 + offsetX;
		int windowPosY = screenCenterY - windowSize.GetHeight() / 2 + offsetY;

		SetPosition(wxPoint(windowPosX, windowPosY));


		cv::Mat imgRGB;
		cv::cvtColor(img, imgRGB, cv::COLOR_BGR2RGB);

		wxImage wxImg(imgRGB.cols, imgRGB.rows, imgRGB.data, true);

		imageCtrl = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImg));

		wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
		vbox->Add(imageCtrl, 1, wxEXPAND | wxALL, 10);
		SetSizer(vbox);

		SetSize(wxSize(imgRGB.cols + 20, imgRGB.rows + 50));

		Show(true);
	}

	~CVImageWindow() {}

	wxStaticBitmap* imageCtrl;
};


fn parse_bool(string str) -> uint
{
	uint kernel = 0;
	auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), kernel);
	if (ec != errc() || kernel > 1) {
		throw runtime_error("Erreur, la valeur doit etre 0 (zero) ou 1 (un).");
	}
	return kernel;
}

struct Mif02Plugin
{
	virtual fn getName() const -> string_view = 0;
	virtual fn setupUi(wxBoxSizer*, wxPanel* ) -> void = 0;
	virtual fn onApply(const cv::Mat&, cv::Mat&) -> void = 0;
	virtual ~Mif02Plugin() = default;
};

vector<unique_ptr<Mif02Plugin>>	plugins;

#define REGISTER_PLUGIN(TypeName)						\
	struct TypeName##Registrar { 						\
		TypeName##Registrar() {							\
			plugins.push_back(make_unique<TypeName>());	\
		}												\
	} TypeName##Instance;

namespace Components
{
	#include "components/BoolInput.hpp"
	#include "components/TextInput.hpp"
	#include "components/IntegerInput.hpp"
	#include "components/OddIntegerInput.hpp"
}

Components::OddIntegerInput kernelSizeComponent { "Taille du kernel: ", 5, 1, 31 };
Components::BoolInput 		useOpenCVComponent 	{ "Utiliser fonction de OpenCV", false };
Components::BoolInput 		useLogNormInHistComponent 	{ "Utiliser normalisation logarithmique dans l'histogramme", false };


#include "addons/filter/average.hpp"
#include "addons/filter/highpass.hpp"
#include "addons/filter/gaussian.hpp"
#include "addons/filter/laplacian.hpp"
#include "addons/filter/sobel.hpp"
#include "addons/filter/median.hpp"


#include "addons/geometric/zoom.hpp"
#include "addons/geometric/rotation.hpp"

#include "addons/histogram/show.hpp"
#include "addons/histogram/equalize.hpp"
#include "addons/histogram/stretch.hpp"
#include "addons/histogram/shift.hpp"
#include "addons/histogram/compression.hpp"


struct Mif02Filters : public wxApp {
	virtual bool OnInit();
};

struct Mif02FiltersFrame : public wxFrame {
	const uint pictures_margin = 0;
	const string_view status_bar_text = "Mif02 - Analyse - TP1 (Image originale à gauche; modifée à droite)";

	Mif02FiltersFrame();

	fn UpdateWindowLayout(bool) -> void;
	fn ShowImage(wxStaticBitmap* widget, const cv::Mat& image) -> void;

	string	current_picture_path;
	wxBoxSizer* mainSizer;
	wxStaticBitmap* beforeImage;
	wxStaticBitmap* afterImage;
	wxBoxSizer* imageSizer;
	wxChoice* filterChoice;
	wxBoxSizer* extensibleSizer;

	cv::Mat loadedImage;
	cv::Mat processedImage;
};

wxIMPLEMENT_APP(Mif02Filters);

fn Mif02FiltersFrame::UpdateWindowLayout(bool on_open_img) -> void {
	if (on_open_img) {
		auto minWidth = beforeImage->GetSize().GetWidth() * 2;
		auto minHeight = beforeImage->GetSize().GetHeight();
		auto width = GetSize().GetWidth();
		auto height = GetSize().GetHeight();
		auto force_resize = false;
		if (width < minWidth) {
			width = minWidth;
			force_resize = true;
		}
		if (height < minHeight) {
			height = minHeight;
			force_resize = true;
		}
		if (force_resize) {
			SetSize(wxSize(width, height));
		}
		SetMinSize(wxSize(minWidth, /*minHeight +*/ 400));
	}
	mainSizer->Layout();
}

fn Mif02Filters::OnInit() -> bool {
	Mif02FiltersFrame* frame = new Mif02FiltersFrame();
	frame->Show(true);
	return true;
}

Mif02FiltersFrame::Mif02FiltersFrame()
	: wxFrame(nullptr, wxID_ANY, "Mif02 - Analyse - TP1", wxDefaultPosition, wxSize(800, 600)) {
	wxMenu* fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN, "&Ouvrir une image\tCtrl-O");
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, "&File");
	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText(wxString(status_bar_text.data(), status_bar_text.size()));

	wxPanel* panel = new wxPanel(this);
	mainSizer = new wxBoxSizer(wxVERTICAL);

	imageSizer = new wxBoxSizer(wxHORIZONTAL);
	beforeImage = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(200, 200));
	afterImage = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(200, 200));
	imageSizer->Add(beforeImage, 1, wxEXPAND | wxALL, pictures_margin);
	imageSizer->Add(afterImage, 1, wxEXPAND | wxALL, pictures_margin);

	mainSizer->Add(imageSizer, 1, wxEXPAND | wxALL, 0);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* filterLabel = new wxStaticText(panel, wxID_ANY, "Fonction:");
	filterChoice = new wxChoice(panel, wxID_ANY);
	filterChoice->Append("------ Choissez un outil -------");
	for (auto &item : plugins)
	{
		auto sv = item->getName();
		filterChoice->Append(wxString(sv.data(), sv.size()));
	}
	filterChoice->SetSelection(0);
	hbox1->Add(filterLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
	hbox1->Add(filterChoice, 1, wxEXPAND);
	vbox->Add(hbox1, 0, wxEXPAND | wxALL, 10);
	mainSizer->Add(vbox, 0, wxEXPAND | wxALL, 10);
	extensibleSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(extensibleSizer, 0, wxEXPAND | wxALL, 10);
	panel->SetSizer(mainSizer);

	filterChoice->Bind(wxEVT_CHOICE, [=](wxCommandEvent& event) {
		extensibleSizer->Clear(true);
		wxString selectedFilter = filterChoice->GetStringSelection();
		Mif02Plugin *plugin = nullptr;
		for (auto& item: plugins)
		{
			auto sv = item->getName();
			if (wxString(sv.data(), sv.size()) == selectedFilter)
			{
				plugin = item.get();
				break ;
			}
		}
		if (!plugin)
			return ;
		std::cout << "SETUP UI" << std::endl;
		std::cout << "END" << std::endl;
		plugin->setupUi(extensibleSizer, panel);
		wxButton* applyButton = new wxButton(panel, wxID_ANY, "Appliquer");
		extensibleSizer->Add(applyButton, 0, wxALIGN_CENTER | wxALL, 10);
		applyButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent& event) {
			if (!loadedImage.empty()) {
				processedImage = loadedImage.clone();
				try {
					auto windows = wxTopLevelWindows;
					for (wxWindow* win : windows) {
						if (win != this) {
							win->Close(false);
						}
					}
					plugin->onApply(loadedImage, processedImage);
					ShowImage(beforeImage, loadedImage);
					ShowImage(afterImage, processedImage);
					UpdateWindowLayout(true);
				}
				catch (const exception& ex) {
					wxString errorMessage = wxString::Format("Erreur : %s", ex.what());
					wxMessageBox(errorMessage, "Error", wxOK | wxICON_INFORMATION);
				}
				catch (...) {
					wxMessageBox("Une erreur inconnue s'est produite", "Info", wxOK | wxICON_INFORMATION);
				}
			} else {
				wxMessageBox( "Merci de d'abord ouvrir une image.", "Error", wxOK | wxICON_ERROR, this);
			}
		});
		mainSizer->Layout();
	});

	Bind(wxEVT_MENU, [=](wxCommandEvent& event) {
		wxFileDialog openFileDialog(this, _("Open Image file"), "", "", "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return;

		loadedImage = cv::imread(current_picture_path = openFileDialog.GetPath().ToStdString());
		if (loadedImage.empty()) {
			wxMessageBox("Failed to load the image", "Error", wxOK | wxICON_ERROR, this);
		} else {
			ShowImage(beforeImage, loadedImage);
			UpdateWindowLayout(true);
		}
	}, wxID_OPEN);

	Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
		mainSizer->Layout();
		event.Skip();
	});

	UpdateWindowLayout(true);
}

fn Mif02FiltersFrame::ShowImage(wxStaticBitmap* widget, const cv::Mat& image) -> void {
	wxSize widgetSize = widget->GetSize();

	int maxWidth = widgetSize.GetWidth();
	int maxHeight = widgetSize.GetHeight();
	double scale = 1.0;
	if (image.cols > maxWidth || image.rows > maxHeight) {
		double scaleX = static_cast<double>(maxWidth) / image.cols;
		double scaleY = static_cast<double>(maxHeight) / image.rows;
		scale = min(scaleX, scaleY);
	}

	auto pic_path = current_picture_path;
	const uint max_picpath_len = 15;
	if (pic_path.length() > max_picpath_len) {
		pic_path = "..." + pic_path.substr(max_picpath_len);
	}
	// TODO:: differencier zoom droit du gauche... laisser option user de partout...
	SetStatusText(wxString::Format("%s | Picture : %s | Zoom : %d%% ", status_bar_text, pic_path.c_str(), (uint)(scale * 100)));


	cv::Mat resizedImage;
	if (scale < 1.0) {
		cv::resize(image, resizedImage, cv::Size(), scale, scale, cv::INTER_AREA);
	} else {
		resizedImage = image;
	}

	cv::Mat tempImage;
	cv::cvtColor(resizedImage, tempImage, cv::COLOR_BGR2RGB);

	wxImage wxImg(tempImage.cols, tempImage.rows, tempImage.data, true);
	widget->SetBitmap(wxBitmap(wxImg));
}
