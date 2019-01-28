QT += core widgets multimedia
CONFIG += console

#TARGET = eqstd
#TEMPLATE = lib
#CONFIG += staticlib
#DESTDIR = ../../lib


DEFINES += QT_DEPRECATED_WARNINGS CURL_STATICLIB


INCLUDEPATH += ./src


win32{
INCLUDEPATH += \
    ./third/win64/curl/include \
    ./third/win64/openssl/include


LIBS += -lWs2_32 -liphlpapi -lWinmm -lUser32 -lAdvapi32 -lGdi32 -lcrypt32 -lwldap32 \
        -L./third/win64/curl/lib \
        -L./third/win64/openssl/lib \
        -lcrypto -lssl -lcurl

QMAKE_LFLAGS = /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmtd.lib


}else:unix{

INCLUDEPATH += \
    ./third/linux64/curl/include \
    ./third/linux6464/openssl/include

LIBS += -lpthread \
        -L./third/linux64/curl/lib \
        -L./third/linux64/openssl/lib \
         -lcurl -lssl -lcrypto -ldl -lz

QMAKE_CFLAGS += -Wno-unused-parameter \
                -Wno-unused-variable \
                -Wno-unused-function \
                -Wno-missing-field-initializers \
                -Wno-deprecated-declarations
}

QMAKE_CXXFLAGS += $$QMAKE_CFLAGS

HEADERS += \
    src/base64.h \
    src/outstream.h \
    src/alioss.h

SOURCES += \
    src/alioss.c \
    main.cpp


SOURCES += \
    test/test_outstream.c \










