#pragma once

#include <QObject>
#include <QString>
#include "opencv2/opencv.hpp"

class PluginInterface
{
public:
    virtual ~PluginInterface() {};
    virtual QString name() = 0;
    virtual void edit(const cv::Mat &input, cv::Mat &output) = 0;
};


#define PLUGIN_INTERFACE_IID "plugininterface"
Q_DECLARE_INTERFACE(PluginInterface, PLUGIN_INTERFACE_IID);

