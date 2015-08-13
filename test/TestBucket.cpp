#include "gtest/gtest.h"

#include <QtCore/QFile>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include "Auth.h"
#include "Bucket.h"

using namespace ASSS;


TEST(Bucket, DISABLED_buckets)
{
    QString accessKey = qgetenv("S3_ACCESS_KEY_ID");
    QString secretKey = qgetenv("S3_SECRET_ACCESS_KEY");

    const Auth auth(accessKey, secretKey);
    const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const QString bucketName = "cbldev";
    Bucket bucket(bucketName);
    bucket.setEndpoint(endpoint);
    bucket.setAuth(auth);

    auto buckets = bucket.buckets();
    for(auto item :buckets)
    {
        qDebug()<<item;
    }
    qDebug()<<"Count:"<<buckets.size();
    ASSERT_NE(0, buckets.size());
}

TEST(Bucket, DISABLED_objects)
{
    QString accessKey = qgetenv("S3_ACCESS_KEY_ID");
    QString secretKey = qgetenv("S3_SECRET_ACCESS_KEY");

    const Auth auth(accessKey, secretKey);
    const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const QString bucketName = "cbldev";
    Bucket bucket(bucketName);
    bucket.setEndpoint(endpoint);
    bucket.setAuth(auth);

    BucketResponse response = bucket.objects();
    ASSERT_EQ("cbldev", response.bucketName());
    ASSERT_EQ(size_t(1000), response.maxKeys());

    const auto buckets = response.objects();
    ASSERT_EQ(1000, buckets.size());
}

TEST(Bucket,  DISABLED_allObjects)
{
    QString accessKey = qgetenv("S3_ACCESS_KEY_ID");
    QString secretKey = qgetenv("S3_SECRET_ACCESS_KEY");

    const Auth auth(accessKey, secretKey);
    const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const QString bucketName = "cbldev";
    Bucket bucket(bucketName);
    bucket.setEndpoint(endpoint);
    bucket.setAuth(auth);

    QString marker;
    bool next = false;
    ObjectInfoList buckets;
    while(true)
    {
        const auto response = bucket.objects(marker);
        if(response.objects().isEmpty())
        {
            break;
        }
        buckets.append(response.objects());

        marker = response.objects().last().key();
    };
    qDebug()<<"Objects:"<<buckets.count();
    ASSERT_TRUE(buckets.count()>52323);
}

TEST(Bucket, DISABLED_download)
{
    QString accessKey = qgetenv("S3_ACCESS_KEY_ID");
    QString secretKey = qgetenv("S3_SECRET_ACCESS_KEY");

    const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const Auth auth(accessKey, secretKey);

    const QString bucketName = "cbldev";
    Bucket bucket(bucketName);
    bucket.setEndpoint(endpoint);
    bucket.setAuth(auth);

    const QList<QPair<QString, QString>> files = {
            {"/23/hwupgradewizard.log", "test.log"},
            {"(04) [Within Temptation] Faster.mp3", "test.mp3"}
    };

    for(auto iter = files.cbegin(); iter!=files.cend(); ++iter)
    {
        ObjectInfo obj(iter->first);
        bucket.download(obj,iter->second);
        qDebug()<<bucket.lastError().message();
    }
}

TEST(Bucket, upload)
{
    QString accessKey = qgetenv("S3_ACCESS_KEY_ID");
    QString secretKey = qgetenv("S3_SECRET_ACCESS_KEY");

    //const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const Endpoint endpoint("eu-west-1","s3-eu-west-1.amazonaws.com");
    const Auth auth(accessKey, secretKey);

    const QString bucketName = "cbldev";
    Bucket bucket(bucketName);
    bucket.setEndpoint(endpoint);
    bucket.setAuth(auth);

#ifdef DATA_DIR
    QString dataDir = QLatin1String(DATA_DIR);
#else
    QString dataDir;
    ASSERT_TRUE(false)<<"No data directory";
#endif

    QFile file(dataDir+"/to_upload.txt");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly ))<<"to_upload.txt not open";
    const QString key("rpg18/to_upload.txt");
    bucket.upload(key, file);
    //bucket.upload("rpg18/", QByteArray());
    ASSERT_TRUE(bucket.lastError().isEmpty());
}
