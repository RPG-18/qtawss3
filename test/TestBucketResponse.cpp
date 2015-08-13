#include "gtest/gtest.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "BucketResponse.h"

using namespace ASSS;

TEST(BucketResponse, parse)
{
#ifdef DATA_DIR
    QString dataDir = QLatin1String(DATA_DIR);
#else
    QString dataDir;
    ASSERT_TRUE(false)<<"No data directory";
#endif

    QFile ans(dataDir+"/ans.xml");
    ASSERT_TRUE(ans.open(QIODevice::ReadOnly | QIODevice::Text))<<"ans.xml not open";

    BucketResponse response;

    response.parse(&ans);

    ASSERT_EQ("cbldev", response.bucketName());
    ASSERT_EQ(size_t(1000), response.maxKeys());
    ASSERT_TRUE(response.isTruncated());

    const auto buckets = response.objects();
    ASSERT_EQ(1000, buckets.size());
}
