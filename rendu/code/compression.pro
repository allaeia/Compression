TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

//CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/main.cpp

linux { # For Linux
  QMAKE_CXXFLAGS += -march=native
  QMAKE_CXXFLAGS_RELEASE *= -O3
}
#for ubuntu et mac
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib

INCLUDEPATH += include


LIBS += -lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann \
-lopencv_nonfree

HEADERS += \
    include/quantif.h \
    include/haar.h \
    include/huffman.h
