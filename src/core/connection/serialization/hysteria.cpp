#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "HysteriaImporter"

namespace Qv2ray::core::connection
{
    namespace serialization::hysteria
    {
        CONFIGROOT Deserialize(const QString &hysteriaUri, QString *alias, QString *errMessage)
        {
            HysteriaServerObject server;
            StreamSettingsObject stream;
            MaskObject udp;
            QString d_name;

            if (hysteriaUri.length() < 12)
            {
                LOG("hysteria2:// string too short");
                *errMessage = QObject::tr("Hysteria2 URI is too short");
            }

            auto url = QUrl::fromUserInput(hysteriaUri);
            if (url.scheme() != "hysteria2")
            {
                LOG("not a trojan share link");
                *errMessage = QObject::tr("Not a Hysteria2 share link!");
            }
            server.address = url.host();
            server.port = url.port();
            server.version = 2;
            stream.hysteriaSettings.auth = url.userInfo();
            if (url.hasFragment())
            {
                d_name = url.fragment(QUrl::FullyDecoded);
            }

            // query
            QUrlQuery query(url.query());
            const auto trueList = QStringList{ "true", "1", "yes", "y" };

            if (query.hasQueryItem("sni"))
            {
                stream.tlsSettings.serverName = query.queryItemValue("sni");
                stream.security = "tls";
            }
            if (query.hasQueryItem("insecure"))
            {
                stream.tlsSettings.allowInsecure = trueList.contains(query.queryItemValue("insecure"));
                stream.security = "tls";
            }
            if (query.hasQueryItem("obfs"))
            {
                udp.type = query.queryItemValue("obfs");
                if (query.hasQueryItem("obfs-password"))
                {
                    udp.settings.password =  query.queryItemValue("obfs-password");
                }
                stream.finalmask.udp.append(udp);
            }
            if (query.hasQueryItem("pinSHA256"))
            {
                QString val = query.queryItemValue("pinSHA256");
                stream.tlsSettings.pinnedPeerCertificateChainSha256.append(val);
            }
            if (query.hasQueryItem("mport") || query.hasQueryItem("ports"))
            {
                stream.finalmask.quicParams.udpHop.ports = query.hasQueryItem("mport") ?
                    query.queryItemValue("mport") : query.queryItemValue("ports");
            }
            stream.finalmask.quicParams.brutalUp = "1000mbps";
            stream.finalmask.quicParams.brutalDown = "1000mbps";
            stream.network = "hysteria";

            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "hysteria", GenerateHysteriaOUT(server), stream.toJson()));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG("Deduced alias: " + *alias);
            return root;
        }

        const QString Serialize(const StreamSettingsObject &stream, const HysteriaServerObject &server, const QString &alias)
        {
            QUrl url;
            url.setUserInfo(stream.hysteriaSettings.auth);
            url.setScheme("hysteria2");
            url.setHost(server.address);
            url.setPort(server.port);
            url.setFragment(alias);

            QUrlQuery query;
            if (!stream.tlsSettings.serverName.isEmpty())
            {
                query.addQueryItem("sni", stream.tlsSettings.serverName);
            }
            if (!stream.tlsSettings.pinnedPeerCertificateChainSha256.isEmpty())
            {
                query.addQueryItem("pinSHA256", stream.tlsSettings.pinnedPeerCertificateChainSha256.first());
            }
            if (!stream.finalmask.udp.isEmpty())
            {
                query.addQueryItem("obfs", stream.finalmask.udp.first().type);
                query.addQueryItem("obfs-password", stream.finalmask.udp.first().settings.password);
            }
            query.addQueryItem("insecure", stream.tlsSettings.allowInsecure ? "1" : "0");
            url.setQuery(query);

            return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
        }
    } // namespace serialization::hysteria
} // namespace Qv2ray::core::connection