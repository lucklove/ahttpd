#!/bin/bash

wget http://www.broadinstitute.org/~carneiro/travis/cmake_3.1.0-rc1-1_amd64.deb
sudo apt-get remove cmake cmake-data
sudo dpkg --install cmake_3.1.0-rc1-1_amd64.deb
