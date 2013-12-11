TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

SOURCES += src/main.cpp \
    src/td2.cpp \
    src/haar.cpp

#for ubuntu et mac
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib

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
    src/td2.h \
    src/haar.h
