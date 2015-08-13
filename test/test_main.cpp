/*
 * test_main.cpp
 *
 *  Created on: 10 авг. 2014 г.
 *      Author: dmitry
 */

#include <QtCore/QCoreApplication>

#include "gtest/gtest.h"

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	::testing::InitGoogleTest(&argc, argv);
	 return RUN_ALL_TESTS();
}
