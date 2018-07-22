#include "chinesedisplaytool.h"
#include <QByteArray>
#include <QString>

ChineseDisplayTool::ChineseDisplayTool()
{

}

QString ChineseDisplayTool::Convert(QString ssContent)
{
    QByteArray bytes = ssContent.toLocal8Bit();
    QString ssMsg;
    ssMsg = QString::fromLocal8Bit(bytes.data());
    return ssMsg;
}
