# Copyright (C) 2020, Yeolar

add_executable(HttpDemo
  HttpDemo.cpp
)
target_link_libraries(HttpDemo crystal)

add_executable(HttpsDemo
  HttpsDemo.cpp
)
target_link_libraries(HttpsDemo crystal)
