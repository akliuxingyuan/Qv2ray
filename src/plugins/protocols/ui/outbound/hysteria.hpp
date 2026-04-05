#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_hysteria.h"

class HysteriaOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::hysteriaOutEditor
{
    Q_OBJECT

  public:
    explicit HysteriaOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &addr, int port) override
    {
        hysteria.address = addr;
        hysteria.port = port;
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { hysteria.address, hysteria.port };
    }

    void SetContent(const QJsonObject &content) override
    {
        PLUGIN_EDITOR_LOADING_SCOPE({
            // hysteria Configs
            hysteria = HysteriaServerObject::fromJson(content);
            versionLineEdit->setText(QString::number(hysteria.version));
        })
    }
    const QJsonObject GetContent() const override
    {
        return hysteria.toJson();
    }

  private:
    HysteriaServerObject hysteria;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_versionLineEdit_textEdited(const QString &arg1);
};
