#! /bin/bash
make clean
make
rm ../test/lda
cp lda ../test
