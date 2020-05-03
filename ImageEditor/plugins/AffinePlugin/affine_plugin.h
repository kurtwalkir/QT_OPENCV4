#include <QObject>
#include <QtPlugin>

#include "plugin_interface.h"

class AffinePlugin: public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_INTERFACE_IID);
    Q_INTERFACES(PluginInterface);
public:
    QString name();
    void edit(const cv::Mat &input, cv::Mat &output);
};
