#include "QRCodeHelper.hpp"

#include "QtQrCodeBuilder.hpp"

#include <QImage>

namespace Qv2ray::ui
{
    QString DecodeQRCode(const QImage &)
    {
        return "";
    }

    QImage EncodeQRCode(const QString content, int size)
    {
        QtQrCodeBuilder qcb;
        auto qrCode = qcb.buildQrCode(content);
        return qcb.drawQrCodeImage(qrCode, size, 2);
    }
} // namespace Qv2ray::ui
