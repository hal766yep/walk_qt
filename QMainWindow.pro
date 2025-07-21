QT       += core gui serialport opengl
CONFIG += c++11
CODECFORTR = UTF-8

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

#  防止出现不能使用glu库的错误
LIBS += -lOpengl32 \
        -lglu32 \

# 支持OpenGL
DEFINES += QCUSTOMPLOT_USE_OPENGL\
           QT_DEPRECATED_WARNINGS \


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#在使用msvc编译器时，中文显示乱码，防止中文乱码
msvc{
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    model_processing.cpp \
    vHeatMap/src/kriginginterpolation.cpp \
    vHeatMap/src/radialbasisfunction.cpp \
    vHeatMap/vheatmap.cpp \
    vQCustomPlot/qcustomplot.cpp \
    vQCustomPlot/vqcustomplot.cpp \
    vQCustomPlot/vtracer.cpp \
    vSerialPortCfg/vcodeconverter.cpp \
    vSerialPortCfg/vqserialrxthread.cpp \
    vSerialPortCfg/vqserialtxthread.cpp \
    vSerialPortCfg/vserialport.cpp \
    vserialcom.cpp


HEADERS += \
    mainwindow.h \
    vHeatMap/src/interpolationfunction.h \
    vHeatMap/src/kriginginterpolation.h \
    vHeatMap/src/radialbasisfunction.h \
    vHeatMap/vheatmap.h \
    vQCustomPlot/qcustomplot.h \
    vQCustomPlot/vqcustomplot.h \
    vQCustomPlot/vtracer.h \
    vSerialPortCfg/vcodeconverter.h \
    vSerialPortCfg/vqserialrxthread.h \
    vSerialPortCfg/vqserialtxthread.h \
    vSerialPortCfg/vserialport.h \
    vserialcom.h


FORMS += \
    mainwindow.ui

INCLUDEPATH += ./vHeatmap \
               ./vHeatmap/src \
               ./vHeatmap/thirdparty \
               ./vQCustomPlot \
               ./vProcess \
               ./vSerialPortCfg \
               ./vPlainTextEdit \
               ./v3DSurface \
               ./vQ3DBar

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    res.qrc
RC_ICONS = myico.ico

#INCLUDEPATH += E:/ChromeDownloads/opencv/build/include
#INCLUDEPATH += E:/ChromeDownloads/opencv/build/include/opencv2

#LIBS += -LE:/ChromeDownloads/opencv/build/x64/vc16/lib\
#        -lopencv_world4110

# 如果用的是 opencv_core、opencv_imgproc 等，也可以单独写 -lopencv_core411 等
CONFIG += c++17
