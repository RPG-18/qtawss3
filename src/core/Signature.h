#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QLatin1String>
#include <QtCore/QCryptographicHash>

#include <QtNetwork/QNetworkAccessManager>

class QUrl;
class QUrlQuery;
class QIODevice;
class QNetworkRequest;

#include "Auth.h"

namespace ASSS
{

    class Signature
    {
    public:

        static const QByteArray SERVICE;
        static const QString DATE_FORMAT;
        static const QString DATE_TIME_FORMAT;
        static const QByteArray ALGORITHM;
        static const QCryptographicHash::Algorithm HASH_ALGORITHM;

        Signature(const Auth& auth,
                const QString& region);

        /*!
         * @see http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-authenticating-requests.html
         */
        void sign(QNetworkRequest& request,
                QNetworkAccessManager::Operation operation,
                const QDateTime& dateTime, const QByteArray& dataSign);

        QByteArray hash(const QByteArray& data) const;
        QByteArray hash(QIODevice& device) const;

        QString canonicalPath(const QUrl& url) const;

    private:

        void setAuthorizationHeader(QNetworkRequest& request,
                QNetworkAccessManager::Operation operation,
                const QDateTime& dateTime);


        QByteArray signingKey(const QDateTime& dateTime);

        QByteArray canonicalRequest(QNetworkAccessManager::Operation operation,
                QNetworkRequest& request);

        QByteArray canonicalQuery(const QUrlQuery& query) const;

        QByteArray credentialScope(const QDateTime &date) const;
        /*!
         * @param[out] signedHeaders
         * @see http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
         */
        QByteArray canonicalHeaders(const QNetworkRequest& request);

        QByteArray canonicalHeader(const QByteArray& headerName,
                const QByteArray& headerValue);

        QByteArray stringToSign(const QDateTime& dateTime, const QByteArray& credential,const QByteArray& canonicalRequest) const;

    private:

        Auth m_auth;
        QString m_region;

        QString m_signedHeaders;
        QByteArray m_dataSign;
    };
}
