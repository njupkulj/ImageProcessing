CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( ImageProcessing )

INCLUDE_DIRECTORIES(include)

ADD_EXECUTABLE( my_DIP src/main.cpp src/Processing.cpp )
