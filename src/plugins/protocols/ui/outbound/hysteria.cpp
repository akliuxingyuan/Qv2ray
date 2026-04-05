#include "hysteria.hpp"

#include "BuiltinProtocolPlugin.hpp"

HysteriaOutboundEditor::HysteriaOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
}

void HysteriaOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void HysteriaOutboundEditor::on_versionLineEdit_textEdited(const QString &arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    hysteria.version = arg1.toInt();
}
