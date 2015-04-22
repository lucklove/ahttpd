#!/bin/bash

sed -i "s/boost_system/boost_system boost_regex/g" src/CMakeLists.txt
sed -i "s/<regex>/<boost\/regex.hpp>/g" src/*
sed -i "s/std::regex/boost::regex/g" src/*
sed -i "s/std::smatch/boost::smatch/g" src/*
