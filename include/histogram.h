#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>

#include <opencv2/opencv.hpp>


/*
 * Un histogram est un tabeau de 256 entiers
 */
using histogram = std::array<unsigned long long, 256>;
using lut = std::array<uchar, 256>;


template <typename T>
T saturate_cast(int value);



/*
 *		Arondi a l'entier au plus proche
 */
int Round(float value);


/*
 *	Compte le nombre de pixel par couleur (entre 0 et 255) pour le channel demande
 */
histogram calcHist(const cv::Mat& img, int channel);


/*
 *  Met l'histogram a l'echelle  demandee
 */
void normalizeHist(uint pixels, histogram &hist, uint histHeight);


/*
 *  Calcule l'histogramme d'une image en utilisant des tableaux histogram pour chaque canal (BGR),
 *	puis le normalise avant de le retourner sous forme d'image ocv avec des labels d'échelle pour les axes.
 */
cv::Mat generateHistImage(const cv::Mat& img, bool logNorm);


/*
 *  Ajustement du contraste en fonction du facteur choisis
 */
cv::Mat equalizeHist(const cv::Mat& img, double contrast_factor = 1.0);


/*
 * Etire lhistogram en fonction du facteur
 */
cv::Mat stretchHist(const cv::Mat& img, double stretch_factor = 1.0);



cv::Mat shiftColors(const cv::Mat& img, int shiftB = 0, int shiftG = 0, int shiftR = 0);



cv::Mat compressImage(const cv::Mat& input, uint numColors);
cv::Mat compressImageWithOpenCV(const cv::Mat& input, uint numColors);


void applyLUTToChannel(cv::Mat& image, const lut& channelLUT, int channelIndex);
void applyLUTsToChannels(cv::Mat& image, const lut& rLUT, const lut& gLUT, const lut& bLUT);
lut calculateLutBetweenImages(const cv::Mat& image1, const cv::Mat& image2) ;
cv::Mat generateLUTImage(const lut& transformation);

#endif
