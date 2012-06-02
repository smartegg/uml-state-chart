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

/**
 * @brief CircleBuf
 *   a CircleBuf  used as a  scatter-gather  way.
 *   can be used easily with  readv() writev()
 *
 * @note I fell strange and uncomfortable to use this class,
 *     just weired.
 */
class CircleBuf
{
	char *buffer_;
	size_t length_;
	size_t idleBegin_;
	size_t idleSize_;

public:
  ///total capacity of this circle-buffer
	size_t capacity()
	{ return length_; }
  ///how many bytes consumed
	size_t size()
	{ return length_ - idleSize_; }
  /// check if this  buffer is empty
	bool isEmpty()
	{ return length_ == idleSize_; }
  /// check if this buffer is full
	bool isFull()
	{ return !idleSize_; }

  ///return all the iovec you can used to  push into data
	void getAppendIov(struct iovec* &iov, int &cnt);

  ///tell the buffer we has pushed into @c len bytes of data,
  ///how to use ?
  /// 1) use  getAppendIov() get to buffer pointer,
  /// 2) push the data into the pointer  position
  /// 3) then use this func to tell how many bytes has copied
	void adjustAfterAppend(size_t len);

  ///return all the iovec that have data in it.
	void getPopIov(struct iovec* &iov, int &cnt);
  ///tell the buffer, we pop @c len bytes data.
	void adjustAfterPop(size_t len);

	CircleBuf(size_t cap);
	~CircleBuf();
};

#endif /* _CIRCLE_BUF_H_ */

