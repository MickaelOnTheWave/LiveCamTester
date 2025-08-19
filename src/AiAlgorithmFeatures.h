#ifndef AIALGORITHMFEATURES_H
#define AIALGORITHMFEATURES_H

#include <string>
#include <vector>
#include <QWidget>

#include "AbstractFaceDetector.h"

struct AiAlgorithmFeatures
{
   QWidget* controlWidget;
   std::vector<std::string> modelFiles;
   AbstractFaceDetector* algorithm;
};

#endif // AIALGORITHMFEATURES_H
