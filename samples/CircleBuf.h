/**
 * @file CircleBuf.h
 * @comment 
 * declares the class CircleBuf
 * 
 * @author niexw
 * @email xiaowen.nie.cn@gmail.com
 */
#ifndef _CIRCLE_BUF_H_
#define _CIRCLE_BUF_H_

#include <sys/uio.h>

class CircleBuf
{
	char *buffer;
	size_t length;
	size_t idleBegin;
	size_t idleSize;

public: 
	size_t capacity()
	{ return length; }
	size_t size()
	{ return length - idleSize; }
	bool isEmpty()
	{ return length == idleSize; }
	bool isFull()
	{ return !idleSize; }
	
	void getAppendIov(struct iovec* &iov, int &cnt);
	void adjustAfterAppend(size_t len);
	void getPopIov(struct iovec* &iov, int &cnt);
	void adjustAfterPop(size_t len);
	
	CircleBuf(size_t size);
	~CircleBuf();
};

#endif /* _CIRCLE_BUF_H_ */

