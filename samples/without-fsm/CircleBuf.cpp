/**
 * @file CircleBuf.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Fri, 01 Jun 2012 00:33:37
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include "CircleBuf.h"

#include <cstdio>
#include <iostream>
#include <cassert>

CircleBuf::CircleBuf(size_t cap)
  : buffer_(new char[cap]),
    length_(cap),
    idleSize_(cap),
    idleBegin_(0) {

}

CircleBuf::~CircleBuf() {
  delete buffer_;
}


void CircleBuf::getAppendIov(struct iovec* &iov, int& cnt) {
  cnt = 0;

  if (isFull()) {
    return;
  }

  printf("in reg_read(): free idx: %d  %d\n", idleBegin_, idleSize_);

  int end = (idleBegin_ +  idleSize_) ;

  if (end > length_) {
    end  = length_;
  }

  iov[cnt].iov_base = buffer_ + idleBegin_;
  iov[cnt].iov_len =  end - idleBegin_;
  cnt++;


  if (idleBegin_ + idleSize_ > length_) {
    iov[cnt].iov_base = buffer_;
    iov[cnt].iov_len = idleSize_ - iov[cnt - 1].iov_len;
    cnt++;
  }
}

void CircleBuf::adjustAfterAppend(size_t len) {
  printf("in invoke_read()  free idx: %d  %d\n", idleBegin_, idleSize_);
  printf("total:%d idle: %d push:%d\n", length_, idleSize_, len);
  assert(len <= idleSize_ && len >= 0);

  idleSize_ -= len;
  idleBegin_ += len;
  idleBegin_ %= length_;
}


void CircleBuf::getPopIov(struct iovec* &iov, int& cnt) {
  cnt = 0;

  if (isEmpty()) {
    return;
  }

  printf("in reg_write() free idx: %d  %d\n", idleBegin_, idleSize_);

  if (idleBegin_ + idleSize_ < length_) {
    iov[cnt].iov_base = buffer_ + idleBegin_ + idleSize_;
    iov[cnt].iov_len =  length_ - idleBegin_ - idleSize_;
    cnt++;
  }

  if (idleBegin_ > 0) {
    if (idleBegin_ + idleSize_ <= length_) {
      iov[cnt].iov_base = buffer_;
      iov[cnt].iov_len =  idleBegin_;
    } else {
      iov[cnt].iov_base = buffer_ + (idleBegin_ + idleSize_ - length_);
      iov[cnt].iov_len =  length_ - idleSize_;
    }

    cnt++;
  }

}

void CircleBuf::adjustAfterPop(size_t len)  {
  printf("in invoke_write: free idx: %d  %d\n", idleBegin_, idleSize_);
  printf("total:%d idle: %d pop:%d\n", length_, idleSize_, len);
  assert(len <= (length_ - idleSize_) && len >= 0);

  idleSize_ += len;
}

