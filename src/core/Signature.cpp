#include <QtCore/QMap>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QUrlQuery>
#include <QtCore/QDateTime>
#include <QtCore/QEventLoop>
#include <QtCore/QTextStream>
#include <QtCore/QFutureWatcher>

#include <QtCore/QMessageAuthenticationCode>

#include <QtConcurrent/QtConcurrentRun>


#include <QtNetwork/QNetworkReply>

#include "Signature.h"

namespace
{
    QMap<QNetworkAccessManager::Operation, QByteArray> HTTP_METHOD =
    {
            { QNetworkAccessManager::HeadOperation, "HEAD" },
            { QNetworkAccessManager::GetOperation, "GET" },
            { QNetworkAccessManager::PutOperation, "PUT" },
            { QNetworkAccessManager::PostOperation, "POST" },
            { QNetworkAccessManager::DeleteOperation, "DELETE" }
    };
}
namespace ASSS
{
    const QByteArray Signature::SERVICE = "s3";
    const QString Signature::DATE_FORMAT = "yyyyMMdd";
    const QString Signature::DATE_TIME_FORMAT = "yyyyMMddThhmmssZ";
    const QByteArray Signature::ALGORITHM = "AWS4-HMAC-SHA256";

    const QCryptographicHash::Algorithm Signature::HASH_ALGORITHM = QCryptographicHash::Sha256;

    Signature::Signature(const Auth& auth, const QString& region) :
            m_auth(auth),
            m_region(region)
    {
    }

    QByteArray Signature::signingKey(const QDateTime& dateTime)
    {
        const auto kDate = QMessageAuthenticationCode::hash(
                dateTime.toString(DATE_FORMAT).toUtf8(),
                "AWS4" + m_auth.secretKey().toUtf8(),
                HASH_ALGORITHM);

        const auto kRegion = QMessageAuthenticationCode::hash(m_region.toUtf8(),
                kDate,
                HASH_ALGORITHM);

        const auto kService = QMessageAuthenticationCode::hash("s3",
                kRegion,
                HASH_ALGORITHM);

        const auto kSigning = QMessageAuthenticationCode::hash("aws4_request",
                kService,
                HASH_ALGORITHM);

        return kSigning;
    }

    void Signature::sign(QNetworkRequest& request,
            QNetworkAccessManager::Operation operation,
            const QDateTime& dateTime, const QByteArray& dataSign)
    {
        m_dataSign = dataSign;
        request.setRawHeader("x-amz-date",
                dateTime.toString(DATE_TIME_FORMAT).toUtf8());
        setAuthorizationHeader(request, operation, dateTime);
    }

    void Signature::setAuthorizationHeader(QNetworkRequest& request,
            QNetworkAccessManager::Operation operation,
            const QDateTime& dateTime)
    {
        const auto canRequest = canonicalRequest(operation, request);
        const auto credScope = credentialScope(dateTime);

        const auto sts = stringToSign(dateTime, credScope, canRequest);
        const auto signKey = signingKey(dateTime);
        const auto signature = QMessageAuthenticationCode::hash(sts,
                signKey, HASH_ALGORITHM);

        QByteArray autHeader;
        autHeader.append("AWS4-HMAC-SHA256 ");
        autHeader.append("Credential=");
        autHeader.append(m_auth.accessKey() + '/');
        autHeader.append(credScope);
        autHeader.append(", SignedHeaders=");
        autHeader.append(m_signedHeaders);
        autHeader.append(", Signature=");
        autHeader.append(signature.toHex());

        request.setRawHeader("Authorization", autHeader);
    }

    QByteArray Signature::stringToSign(const QDateTime& dateTime,
            const QByteArray& credentialScope,
            const QByteArray& canonicalRequest) const
    {
        QByteArray sts;
        sts.append(ALGORITHM + '\n');
        sts.append(dateTime.toString(DATE_TIME_FORMAT).toUtf8() + '\n');
        sts.append(credentialScope + '\n');
        sts.append(QCryptographicHash::hash(canonicalRequest, HASH_ALGORITHM).toHex());

        return sts;
    }

    QByteArray Signature::canonicalRequest(
            QNetworkAccessManager::Operation operation,
            QNetworkRequest& request)
    {

        QByteArray canRequest;

        canRequest.append(HTTP_METHOD[operation] + '\n');
        canRequest.append(canonicalPath(request.url()).toUtf8() + '\n');
        canRequest.append(canonicalQuery(QUrlQuery(request.url())) + '\n');
        canRequest.append(canonicalHeaders(request) + '\n');
        canRequest.append(m_signedHeaders + '\n');

        // add body signature
        canRequest.append(m_dataSign);

        return canRequest;
    }

    QByteArray Signature::hash(const QByteArray& data) const
    {
        return QCryptographicHash::hash(data, HASH_ALGORITHM).toHex();
    }

    QByteArray Signature::hash(QIODevice& device) const
    {
        device.seek(0);
        QCryptographicHash hash(HASH_ALGORITHM);

        QFutureWatcher<void> watcher;
        QEventLoop loop;
        QObject::connect(&watcher, &QFutureWatcher<void>::finished,
                         &loop, &QEventLoop::quit);

        watcher.setFuture(QtConcurrent::run([&](){
            hash.addData(&device);
        }));
        loop.exec();
        device.seek(0);
        return hash.result().toHex();
    }

    QString Signature::canonicalPath(const QUrl& url) const
    {
        QString path = url.path(QUrl::FullyEncoded);
        if(path.isEmpty())
        {
            path="/";
        }

        if ((url.path().endsWith(QLatin1Char('/')))
                && (!path.endsWith(QLatin1Char('/'))))
        {
            path += QLatin1Char('/');
        }

        path.replace("(","%28");
        path.replace(")","%29");
        path.replace("[","%5B");
        path.replace("]","%5D");
        path.replace(":","%3A");
        path.replace("&","%26");
        path.replace("+","%20");
        path.replace("!","%21");
        path.replace("@","%40");
        path.replace(",","%2C");

        return path;
    }

    QByteArray Signature::canonicalQuery(const QUrlQuery &query) const
    {
        typedef QPair<QString, QString> StringPair;
        QList<StringPair> list = query.queryItems(QUrl::FullyDecoded);

        qSort(list);
        QString result;
        for (const auto& pair : list)
        {
            if (!result.isEmpty())
            {
                result += QLatin1Char('&');
            }
            result += QString::fromUtf8(QUrl::toPercentEncoding(pair.first))
                    + QLatin1Char('=')
                    + QString::fromUtf8(QUrl::toPercentEncoding(pair.second));
        }
        return result.toUtf8();
    }

    QByteArray Signature::credentialScope(const QDateTime& date) const
    {
        return date.toString(DATE_FORMAT).toUtf8() + "/" + m_region.toUtf8()
                + "/" + SERVICE + "/aws4_request";
    }

    QByteArray Signature::canonicalHeaders(const QNetworkRequest& request)
    {
        QMap<QByteArray, QByteArray> headers;
        //insertion sort
        for (const auto& rawHeader : request.rawHeaderList())
        {
            headers.insert(rawHeader.toLower(), request.rawHeader(rawHeader));
        }

        QByteArray canonicalHeaders;
        for (auto iter = headers.constBegin(); iter != headers.constEnd();
                ++iter)
        {

            canonicalHeaders.append(canonicalHeader(iter.key(), iter.value()));
            canonicalHeaders.append('\n');
            if (!m_signedHeaders.isEmpty())
            {
                m_signedHeaders.append(';');
            }
            m_signedHeaders.append(iter.key());
        }

        return canonicalHeaders;
    }

    QByteArray Signature::canonicalHeader(const QByteArray& headerName,
            const QByteArray& headerValue)
    {
        QByteArray header;
        header.append(headerName.toLower() + ":");
        const auto trimmedValue = headerValue.trimmed();
        bool isInQuotes = false;
        char previousChar = '\0';
        for (int index = 0; index < trimmedValue.size(); ++index)
        {
            char thisChar = trimmedValue[index];
            header.append(thisChar);
            if (isInQuotes)
            {
                if ((thisChar == '"') && (previousChar != '\\'))
                    isInQuotes = false;
            }
            else
            {
                if ((thisChar == '"') && (previousChar != '\\'))
                {
                    isInQuotes = true;
                }
                else if (isspace(thisChar))
                {
                    while ((index < trimmedValue.size() - 1)
                            && (isspace(trimmedValue[index + 1])))
                        ++index;
                }
            }
            previousChar = thisChar;
        }

        return header;
    }
}
