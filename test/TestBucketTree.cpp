#include "gtest/gtest.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "BucketTree.h"
#include "BucketResponse.h"

using namespace Gui;
using namespace ASSS;

TEST(BucketTree, buildAndSaveDot)
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

    BucketTree tree;
    tree.build(response.objects());
    tree.toDot("test.dot");
}
