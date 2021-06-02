cmake_minimum_required(VERSION 3.17)
project(InformationSecurityMIET)

set(CMAKE_CXX_STANDARD 20)

add_executable(InformationSecurityMIET main.cpp RSA.cpp)
target_include_directories(InformationSecurityMIET PUBLIC ./)