/**
 * @file CircleBufTest.cc
 * @comment 
 * implement the test cases of circle buffer
 * 
 * @author niexw
 * @email xiaowen.nie.cn@gmail.com
 */
#include <boost/test/test_tools.hpp>
#include <string>
#include <iostream>

#include "misc/CircleBuf.h"
#include "CircleBufTest.h"

using namespace std;

void CircleBufTestCases::testBuffer()
{
	CircleBuf buffer(4);

	BOOST_CHECK(buffer.size() == 0);
	BOOST_CHECK(buffer.capacity() == 4);

	struct iovec iov[2];
	struct iovec *iovp = iov;
	int cnt;
	buffer.getAppendIov(iovp, cnt);
	BOOST_CHECK(cnt == 1);
	BOOST_CHECK(iov[0].iov_len = 4);
	void *pc = iov[0].iov_base;

	buffer.adjustAfterAppend(1);
	BOOST_CHECK(buffer.size() == 1);

	buffer.getPopIov(iovp, cnt);
	BOOST_CHECK(cnt == 1);
	BOOST_CHECK(iov[0].iov_len = 1);
	BOOST_CHECK(iov[0].iov_base = pc);

	buffer.adjustAfterPop(1);
	BOOST_CHECK(buffer.size() == 0);

	buffer.getAppendIov(iovp, cnt);
	BOOST_CHECK(cnt == 2);
	BOOST_CHECK(iov[0].iov_base = (char*)pc+1);
	BOOST_CHECK(iov[0].iov_len = 3);
	BOOST_CHECK(iov[1].iov_base = pc);
	BOOST_CHECK(iov[1].iov_len = 1);

	buffer.adjustAfterAppend(2);
	BOOST_CHECK(buffer.size() == 2);

	buffer.getPopIov(iovp, cnt);
	BOOST_CHECK(cnt == 1);
	BOOST_CHECK(iov[0].iov_base = (char*)pc+1);
	BOOST_CHECK(iov[0].iov_len = 2);

	buffer.adjustAfterPop(1);
	BOOST_CHECK(buffer.size() == 1);
	
	buffer.getAppendIov(iovp, cnt);
	BOOST_CHECK(cnt == 2);
	BOOST_CHECK(iov[0].iov_base = (char*)pc+3);
	BOOST_CHECK(iov[0].iov_len = 1);
	BOOST_CHECK(iov[1].iov_base = pc);
	BOOST_CHECK(iov[1].iov_len = 2);
	
	buffer.adjustAfterAppend(2);
	BOOST_CHECK(buffer.size() == 3);

	buffer.getPopIov(iovp, cnt);
	BOOST_CHECK(cnt == 2);
	BOOST_CHECK(iov[0].iov_base = (char*)pc+2);
	BOOST_CHECK(iov[0].iov_len = 2);
	BOOST_CHECK(iov[1].iov_base = pc);
	BOOST_CHECK(iov[1].iov_len = 1);

	buffer.adjustAfterPop(2);
	BOOST_CHECK(buffer.size() == 1);

	buffer.getAppendIov(iovp, cnt);
	BOOST_CHECK(cnt == 1);
	BOOST_CHECK(iov[0].iov_base = (char*)pc+1);
	BOOST_CHECK(iov[0].iov_len = 3);

	buffer.adjustAfterAppend(3);
	BOOST_CHECK(buffer.size() == 4);
}

