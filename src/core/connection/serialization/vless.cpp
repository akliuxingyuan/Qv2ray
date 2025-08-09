#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "VLESSImporter"

namespace Qv2ray::core::connection
{
    namespace serialization::vless
    {
        CONFIGROOT Deserialize(const QString &str, QString *alias, QString *errMessage)
        {
            // must start with vless://
            if (!str.startsWith("vless://"))
            {
                *errMessage = QObject::tr("VLESS link should start with vless://");
                return CONFIGROOT();
            }

            // parse url
            QUrl url(str);
            if (!url.isValid())
            {
                *errMessage = QObject::tr("link parse failed: %1").arg(url.errorString());
                return CONFIGROOT();
            }

            // fetch host
            const auto hostRaw = url.host();
            if (hostRaw.isEmpty())
            {
                *errMessage = QObject::tr("empty host");
                return CONFIGROOT();
            }
            const auto host = (hostRaw.startsWith('[') && hostRaw.endsWith(']')) ? hostRaw.mid(1, hostRaw.length() - 2) : hostRaw;

            // fetch port
            const auto port = url.port();
            if (port == -1)
            {
                *errMessage = QObject::tr("missing port");
                return CONFIGROOT();
            }

            // fetch remarks
            const auto remarks = url.fragment();
            if (!remarks.isEmpty())
            {
                *alias = remarks;
            }

            // fetch uuid
            const auto uuid = url.userInfo();
            if (uuid.isEmpty())
            {
                *errMessage = QObject::tr("missing uuid");
                return CONFIGROOT();
            }

            // initialize QJsonObject with basic info
            VLESSServerObject server;
            StreamSettingsObject stream;

            // parse query
            QUrlQuery query(url.query());

            // handle type
            const auto hasType = query.hasQueryItem("type");
            const auto type = hasType ? query.queryItemValue("type") : "tcp";
            if (type != "tcp")
                stream.network = type;

            // handle encryption
            const auto hasEncryption = query.hasQueryItem("encryption");
            const auto encryption = hasEncryption ? query.queryItemValue("encryption") : "none";
            server.users.first().encryption = encryption;

            // type-wise settings
            if (type == "kcp")
            {
                const auto hasSeed = query.hasQueryItem("seed");
                if (hasSeed)
                    stream.kcpSettings.seed = query.queryItemValue("seed");

                const auto hasHeaderType = query.hasQueryItem("headerType");
                const auto headerType = hasHeaderType ? query.queryItemValue("headerType") : "none";
                if (headerType != "none")
                    stream.kcpSettings.header.type = headerType;
            }
            else if (type == "http")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue("path").toUtf8()) : "/";
                if (path != "/")
                    stream.httpSettings.path = path;

                const auto hasHost = query.hasQueryItem("host");
                if (hasHost)
                {
                    const auto hosts = query.queryItemValue("host").split(",");
                    stream.httpSettings.host = hosts;
                }
            }
            else if (type == "xhttp")
            {
                // TODO: xhttp sharelink standard
            }
            else if (type == "ws")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue("path").toUtf8()) : "/";
                if (path != "/")
                    stream.wsSettings.path = path;

                const auto hasHost = query.hasQueryItem("host");
                if (hasHost)
                    stream.wsSettings.headers["host"] = query.queryItemValue("host");
            }
            else if (type == "quic")
            {
                const auto hasQuicSecurity = query.hasQueryItem("quicSecurity");
                if (hasQuicSecurity)
                {
                    const auto quicSecurity = query.queryItemValue("quicSecurity");
                    stream.quicSettings.security = quicSecurity;

                    if (quicSecurity != "none")
                        stream.quicSettings.key = query.queryItemValue("key");

                    const auto hasHeaderType = query.hasQueryItem("headerType");
                    const auto headerType = hasHeaderType ? query.queryItemValue("headerType") : "none";
                    if (headerType != "none")
                        stream.quicSettings.header.type = headerType;
                }
            }
            else if (type == "grpc")
            {
                const auto hasServiceName = query.hasQueryItem("serviceName");
                if (hasServiceName)
                {
                    const auto serviceName = QUrl::fromPercentEncoding(query.queryItemValue("serviceName").toUtf8());
                    stream.grpcSettings.serviceName = serviceName;
                }

                const auto hasMode = query.hasQueryItem("mode");
                if (hasMode)
                {
                    const auto multiMode = QUrl::fromPercentEncoding(query.queryItemValue("mode").toUtf8()) == "multi";
                    stream.grpcSettings.multiMode = multiMode;
                }
            }

            // tls-wise settings
            const auto hasSecurity = query.hasQueryItem("security");
            const auto security = hasSecurity ? query.queryItemValue("security") : "none";
            const auto tlsKey = security == "tls" ? "tlsSettings" : "realitySettings";
            if (security != "none")
                stream.security = security;

            if (security == "reality")
            {
                // flow
                const auto flow = query.queryItemValue("flow");
                server.users.first().flow = flow;

                // sni
                const auto hasSNI = query.hasQueryItem("sni");
                if (hasSNI)
                {
                    const auto sni = query.queryItemValue("sni");
                    stream.realitySettings.serverName = sni;
                }

                // reality settings
                if (query.hasQueryItem("fp"))
                {
                    const auto fp = QUrl::fromPercentEncoding(query.queryItemValue("fp").toUtf8());
                    stream.realitySettings.fingerprint = fp;
                }
                if (query.hasQueryItem("pbk"))
                {
                    const auto pbk = QUrl::fromPercentEncoding(query.queryItemValue("pbk").toUtf8());
                    stream.realitySettings.publicKey = pbk;
                }
                if (query.hasQueryItem("spiderX"))
                {
                    const auto spiderX = QUrl::fromPercentEncoding(query.queryItemValue("spiderX").toUtf8());
                    stream.realitySettings.spiderX = spiderX;
                }
                if (query.hasQueryItem("sid"))
                {
                    const auto sid = QUrl::fromPercentEncoding(query.queryItemValue("sid").toUtf8());
                    stream.realitySettings.shortId = sid;
                }
            } else if (security == "tls") {
                // sni
                const auto hasSNI = query.hasQueryItem("sni");
                if (hasSNI)
                {
                    const auto sni = query.queryItemValue("sni");
                    stream.tlsSettings.serverName = sni;
                }

                // alpn
                const auto hasALPN = query.hasQueryItem("alpn");
                if (hasALPN)
                {
                    const auto alpnRaw = QUrl::fromPercentEncoding(query.queryItemValue("alpn").toUtf8());
                    const auto alpnArray = alpnRaw.split(",");
                    stream.tlsSettings.alpn = alpnArray;
                }
            }

            // assembling config
            CONFIGROOT root;
            OUTBOUNDSETTING vConf;
            OUTBOUNDS outbounds;
            vConf["vnext"] = QJsonArray{ server.toJson() };
            outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "vless", vConf , stream.toJson()));
            JADD(outbounds);

            return root;
        }

        const QString Serialize(const StreamSettingsObject &stream, const VLESSServerObject &server, const QString &alias)
        {
            QUrl url;
            url.setFragment(QUrl::toPercentEncoding(alias));
            url.setScheme("vless");
            url.setHost(server.address);
            url.setPort(server.port);
            url.setUserName(server.users.first().id);

            QUrlQuery query;
            const auto encryption = server.users.first().encryption;
            if (encryption != "none")
                query.addQueryItem("encryption", encryption);

            const auto network = stream.network;
            if (network != "tcp")
                query.addQueryItem("type", network);

            const auto security = stream.security;
            if (security != "none")
                query.addQueryItem("security", security);

            if (network == "kcp")
            {
                const auto seed = stream.kcpSettings.seed;
                if (!seed.isEmpty())
                    query.addQueryItem("seed", QUrl::toPercentEncoding(seed));

                const auto headerType = stream.kcpSettings.header.type;
                if (headerType != "none")
                    query.addQueryItem("headerType", headerType);
            }
            else if (network == "http")
            {
                const auto path = stream.httpSettings.path;
                query.addQueryItem("path", QUrl::toPercentEncoding(path));

                const auto hosts = stream.httpSettings.host;
                query.addQueryItem("host", QUrl::toPercentEncoding(hosts.join(",")));
            }
            else if (network == "ws")
            {
                const auto path = stream.wsSettings.path;
                query.addQueryItem("path", QUrl::toPercentEncoding(path));

                const auto host = stream.wsSettings.headers["host"];
                query.addQueryItem("host", host);
            }
            else if (network == "quic")
            {
                const auto quicSecurity = stream.quicSettings.security;
                if (quicSecurity != "none")
                {
                    query.addQueryItem("quicSecurity", quicSecurity);

                    const auto key = stream.quicSettings.key;
                    query.addQueryItem("key", QUrl::toPercentEncoding(key));

                    const auto headerType = stream.quicSettings.header.type;
                    if (headerType != "none")
                        query.addQueryItem("headerType", headerType);
                }
            }
            else if (network == "grpc")
            {
                auto serviceName = QString("GunService");
                if (!stream.grpcSettings.serviceName.isEmpty())
                    serviceName = stream.grpcSettings.serviceName;
                if (serviceName != "GunService")
                    query.addQueryItem("serviceName", QUrl::toPercentEncoding(serviceName));

                const auto multiMode = stream.grpcSettings.multiMode;
                if (multiMode)
                    query.addQueryItem("mode", "multi");
            }

            if (security == "reality")
            {
                const auto flow = server.users.first().flow;
                query.addQueryItem("flow", flow);

                const auto sni = stream.realitySettings.serverName;
                if (!sni.isEmpty())
                    query.addQueryItem("sni", sni);
            }
            else if (security == "tls")
            {
                const auto sni = stream.realitySettings.serverName;
                if (!sni.isEmpty())
                    query.addQueryItem("sni", sni);

                const auto alpnList = stream.tlsSettings.alpn;
                query.addQueryItem("alpn", QUrl::toPercentEncoding(alpnList.join(",")));
            }

            url.setQuery(query);
            return url.toString(QUrl::FullyEncoded);
        }
    } // namespace serialization::vless
} // namespace Qv2ray::core::connection
