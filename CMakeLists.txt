project(cocos2dx-xxtea)
cmake_minimum_required(VERSION 2.6)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_executable(xxtea src/xxtea.c src/main.c src/xxtea.h src/getopt.c)
else()
  add_executable(xxtea src/xxtea.c src/main.c src/xxtea.h)
endif()
