#!/bin/sh
g++ -g -c *.cpp 
rm test.o
ar  rvs fsm.a  *.o
mv fsm.a samples/
