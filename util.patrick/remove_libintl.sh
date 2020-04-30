#!/bin/bash

for f in `find . -name Makefile`;
do
    sed -i 's/-lintl//g' $f
done
