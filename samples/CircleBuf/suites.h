/**
 * @file suites.h
 * @comment
 *	All tests
 *
 * @author niexw
 */
#ifndef _XCOM_TEST_SUITES_H_
#define _XCOM_TEST_SUITES_H_

#include "CircleBufTest.h"
class MyTestSuit : public test_suite
{
public:
	MyTestSuit() : test_suite("MyTestSuit")
	{
		boost::shared_ptr<CircleBufTestCases> instance(new (CircleBufTestCases));
    add(BOOST_CLASS_TEST_CASE(&CircleBufTestCases::testBuffer, instance));
	}
};

#endif
