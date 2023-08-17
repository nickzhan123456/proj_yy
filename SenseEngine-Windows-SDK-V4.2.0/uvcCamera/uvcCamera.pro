#-------------------------------------------------
#
# Project created by QtCreator 2019-06-28T18:05:24
#
#-------------------------------------------------

#QT += core gui
QT += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uvcCamera
TEMPLATE = app

win32{
LIBS += -lWs2_32 \
        -lWinSpool \
        -lOle32 \
        -luuid \
        -lOleAut32 \
        -lAdvapi32 \
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#LINUX64 RASPBERRY4B TINKERBOARD2S
DEFINES += LINUX64

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
#console

CONFIG += WarningsAsErrorsOn

# macro define ffmpeg platform for library platform prefix default null
FFMPEG_PLATFORM =
message(FFMPEG_PLATFORM : $${FFMPEG_PLATFORM})

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
INCLUDEPATH +=  $$PWD/../ffmpeg/include \
               $$PWD/qmtc \
               $$PWD

DEPENDPATH +=  $$PWD/../ffmpeg/include \
               $$PWD/qmtc \
               $$PWD

HEADERS += \
        XFFmpeg.h \
        XVideoThread.h \
        addbatchwidget.h \
        $$PWD/../include/ai_camera.h \
        antiflickerdialog.h \
        globaltype.h \
        inputdialog.h \
        irlightconfigdialog.h \
        logsettingdialog.h \
        mainwindow.h \
        qmtc/mtcinterface.h \
        qmtc/mtcmanager.h \
        qmtc/parseimagedatathread.h \
        qrcodeswitchdialog.h \
        setrecognizecount.h \
        settingwidget.h \
        upgradewidget.h \
        videowidget.h

SOURCES += \
        XFFmpeg.cpp \
        XVideoThread.cpp \
        addbatchwidget.cpp \
        antiflickerdialog.cpp \
        inputdialog.cpp \
        irlightconfigdialog.cpp \
        logsettingdialog.cpp \
        main.cpp \
        mainwindow.cpp \
        qmtc/mtcinterface.cpp \
        qmtc/mtcmanager.cpp \
        qmtc/parseimagedatathread.cpp \
        qrcodeswitchdialog.cpp \
        setrecognizecount.cpp \
        settingwidget.cpp \
        upgradewidget.cpp \
        videowidget.cpp


FORMS += \
        addbatchwidget.ui \
        antiflickerdialog.ui \
        inputdialog.ui \
        irlightconfigdialog.ui \
        logsettingdialog.ui \
        mainwindow.ui \
        qrcodeswitchdialog.ui \
        setrecognizecount.ui \
        settingwidget.ui \
        upgradewidget.ui

linux {
    linux-g++ | linux-g++-64 | linux-g++-32 | linux-clang | linux-openwrt-g++ | linux-poky-g++ | linux-oe-g++ | linux-aarch64-gnu-g++ {
        message("Linux build")
        CONFIG  += LinuxBuild
        DEFINES += LinuxBuild
        linux-g++ {
            contains(DEFINES, LINUX64) {
                message("Linux linux64_14")
                DEFINES += linux64_14
                FFMPEG_PLATFORM = linux64_14
            }
            contains(DEFINES, RASPBERRY4B) {
                message("Linux arm32_pi4")
                DEFINES += arm32_pi4
                FFMPEG_PLATFORM = arm32_pi4
            }
            contains(DEFINES, TINKERBOARD2S) {
                message("Linux aarch64_tk2s")
                DEFINES += aarch64_tk2s
                FFMPEG_PLATFORM = aarch64_tk2s
            }
        }
        linux-clang {
            message("Linux clang")
            QMAKE_CXXFLAGS += -Qunused-arguments -fcolor-diagnostics
            DEFINES += clang64_14
            FFMPEG_PLATFORM = clang64_14
        }
        linux-openwrt-g++ {
            message("Linux wukong32")
            DEFINES += wukong32
            FFMPEG_PLATFORM = wukong32
        }
        linux-poky-g++ {
            message("Linux wukong64")
            DEFINES += wukong64
            FFMPEG_PLATFORM = wukong64
        }
        linux-oe-g++ {
            message("Linux aarch64_cmitech")
            DEFINES += aarch64_cmitech
            FFMPEG_PLATFORM = aarch64_cmitech
        }
        linux-aarch64-gnu-g++ {
            message("Linux aarch64_skyj")
            DEFINES += aarch64_skyj
            FFMPEG_PLATFORM = aarch64_skyj
        }
    } else {
        error("Unsuported Linux toolchain, only GCC 32- or 64-bit is supported")
    }
}else : win32{
        message("Windows build")
        CONFIG += WindowsBuild
        DEFINES += WindowsBuild
        contains(QT_ARCH, x86_64){
            win32:mingw{
                error("Unsuported Windows mingw 64bit toolchain, only msvc 64bit is supported")
            }
            win32:msvc{
                FFMPEG_PLATFORM = windows-msvc-64bit
            }
        }else {
            win32:mingw{
                FFMPEG_PLATFORM = windows-mingw-32bit
            }
            win32:msvc{
                error("Unsuported Windows msvc 32bit toolchain, only mingw 32bit is supported")
            }
        }
} else {
    error("Unsupported build platform, only Linux, Windows are supported")
}

message(Qt version $$[QT_VERSION])

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc


WindowsBuild{
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
    message(Windows libs files)
    message(+++++++++++++++++++++++++++++++++++++++++++++++)

#    contains(DEFINES, WIN64) {

#    }else{
#        RC_FILE = uvcCamera.rc
#    }

    #Copy the path Settings after compilation
    SourcePath = $$PWD/..
    #Replace the "/" with "\" in SourcePath
    SourcePath = $$replace(SourcePath, /, \\)
    #Replace the "/" with "\"
    OutPath = $$OUT_PWD/
    OutPath = $$replace(OutPath, /, \\)
    CONFIG(debug, debug|release) {
        CurrentConfig = debug
        CurrentLibConfig = libDebug
    } else {
        CurrentConfig = release
        CurrentLibConfig = libRelease
    }

   LIBS += -L$$PWD/../ffmpeg/lib/$${FFMPEG_PLATFORM} \
        -lavutil \
        -lavcodec \
        -lswscale \
        -lavformat \
        -lavdevice \
        -lswresample \
        -lavfilter

    LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/$${CurrentLibConfig} -lAICameraModule

    QMAKE_POST_LINK += \
       echo d | xcopy /y $$SourcePath\\ffmpeg\\bin\\$${FFMPEG_PLATFORM}\\*.dll $$OutPath\\$$CurrentConfig\\  && \
       echo d | xcopy /y $$SourcePath\\lib\\$${FFMPEG_PLATFORM}\\$$CurrentLibConfig\\*.dll $$OutPath\\$$CurrentConfig\\ &&\
       echo d | xcopy /y $$SourcePath\\ffmpeg\\bin\\$${FFMPEG_PLATFORM}\\*.dll $$SourcePath\\releaseVersion\\$${FFMPEG_PLATFORM}\\  && \
       echo d | xcopy /y $$SourcePath\\lib\\$${FFMPEG_PLATFORM}\\$$CurrentLibConfig\\*.dll $$SourcePath\\releaseVersion\\$${FFMPEG_PLATFORM}\\ &&\
       echo d | xcopy /y $$OutPath\\$$CurrentConfig\\*.exe $$SourcePath\\releaseVersion\\$${FFMPEG_PLATFORM}\\ \
}

LinuxBuild{
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
    message(LinuxBuild libs files)
    message(+++++++++++++++++++++++++++++++++++++++++++++++)

    #Copy the path Settings after compilation
    SourcePath = $$PWD/..
    OutPath = $$OUT_PWD
    CONFIG(debug, debug|release) {
        CurrentConfig = debug
        CurrentLibConfig = libDebug
    } else {
        CurrentConfig = release
        CurrentLibConfig = libRelease
    }

    contains(DEFINES, wukong32) {
        message("LinuxBuild wukong32")
        STAGING_DIR = /home/vagrant/wukong/wukong-sdk/staging_dir/target-arm_cortex-a7+neon-vfpv4_glibc_eabi
        TOOLCHAIN_DIR = /home/vagrant/wukong/wukong-sdk/staging_dir/toolchain-arm_cortex-a7+neon-vfpv4_gcc-8.3.0_glibc_eabi/lib
        CONFIG(debug, debug|release) {
            LIBS += -L$${STAGING_DIR}/usr/lib \
                -lgthread-2.0 \
                -lglib-2.0 \
                -ldrm \
                -lavutil \
                -lavcodec \
                -lavformat \
                -lavdevice \
                -lswresample \
                -lavfilter \
                -lswscale \
                -lbz2 \
                -lz \
                -lrga

             LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/debug -lAICameraModule
        }
    }
    contains(DEFINES, wukong64) {
        message("LinuxBuild wukong64")
        STAGING_DIR = /opt/poky/2.5.1/sysroots/aarch64-poky-linux
        TOOLCHAIN_DIR =
         LIBS += -L$${STAGING_DIR}/usr/lib \
                -lavutil \
                -lavcodec \
                -lswscale \
                -lavformat \
                -lavdevice \
                -lswresample \
                -lavfilter \
                -lbz2


        CONFIG(debug, debug|release) {
             LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/debug -lAICameraModule
        } else {
            LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/release -lAICameraModule
        }
    }

    contains(DEFINES, aarch64_cmitech) {
        message("LinuxBuild aarch64_cmitech")
        STAGING_DIR = /opt/poky/2.5.1/sysroots/aarch64-poky-linux
        TOOLCHAIN_DIR =

        INCLUDEPATH += $${STAGING_DIR}/usr/include

        DEPENDPATH += $${STAGING_DIR}/usr/include

        LIBS += -L$$PWD/../ffmpeg/lib/$${FFMPEG_PLATFORM} \
                -lavutil \
                -lavcodec \
                -lswscale \
                -lavformat \
                -lavdevice \
                -lswresample \
                -lavfilter \
                -lbz2


        CONFIG(debug, debug|release) {
             LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/debug -lAICameraModule
        } else {
            LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/release -lAICameraModule
        }


    }
    contains(DEFINES, aarch64_skyj) {
        message("LinuxBuild aarch64_skyj")
        STAGING_DIR = /opt/buildroot
        TOOLCHAIN_DIR =

        INCLUDEPATH += $${STAGING_DIR}/usr/include

        DEPENDPATH += $${STAGING_DIR}/usr/include

        LIBS += -L$$PWD/../ffmpeg/lib/$${FFMPEG_PLATFORM} \
                -lavutil \
                -lavcodec \
                -lswscale \
                -lavformat \
                -lavdevice \
                -lswresample \
                -lavfilter \
                -lbz2


        CONFIG(debug, debug|release) {
             LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/debug -lAICameraModule
        } else {
            LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/release -lAICameraModule
        }


    } else {
        message("FFMPEG_PLATFORM = $${FFMPEG_PLATFORM}")
        LIBS += -L$$PWD/../ffmpeg/lib/$${FFMPEG_PLATFORM} \
            -lavutil \
            -lavcodec \
            -lswscale \
            -lavformat \
            -lavdevice \
            -lswresample \
            -lavfilter

        LIBS += -L$$PWD/../lib/$${FFMPEG_PLATFORM}/$${CurrentLibConfig} -lAICameraModule
    }


    # Version information
    VERSION = 4.2.0
    # ICON
    #RC_ICONS = Resources/sensetime.png
    # COMPANY NAME
    QMAKE_TARGET_COMPANY = "Shenzhen SenseTime Co.,Ltd"
    # PRODUCT NAME
    QMAKE_TARGET_PRODUCT = "SenseEngine Demo"
    # Copyright information
    QMAKE_TARGET_COPYRIGHT = "2018-2028,Shenzhen SenseTime Co.,Ltd"

    QMAKE_CC += -g
    QMAKE_CXX += -g
    QMAKE_LINK += -g   

    QMAKE_PRE_LINK  += rm -rf $$OutPath/libAICameraModule.so*;rm -rf $$SourcePath/releaseVersion/$${FFMPEG_PLATFORM}/libAICameraModule.so*;
    QMAKE_POST_LINK += cp -df $$SourcePath/ffmpeg/lib/$${FFMPEG_PLATFORM}/*.so* $$OutPath/ && \
                       cp -df $$SourcePath/lib/$${FFMPEG_PLATFORM}/$${CurrentLibConfig}/*.so* $$OutPath/ && \
                       cp -df $$SourcePath/ffmpeg/lib/$${FFMPEG_PLATFORM}/*.so* $$SourcePath/releaseVersion/$${FFMPEG_PLATFORM}/ && \
                       cp -df $$SourcePath/lib/$${FFMPEG_PLATFORM}/$${CurrentLibConfig}/*.so* $$SourcePath/releaseVersion/$${FFMPEG_PLATFORM}/ && \
                       cp -df $$OutPath/uvcCamera $$SourcePath/releaseVersion/$${FFMPEG_PLATFORM}/ \
}



