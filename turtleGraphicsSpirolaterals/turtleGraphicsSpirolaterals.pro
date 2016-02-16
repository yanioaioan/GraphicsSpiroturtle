TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
LIBS += -L/usr/lib -lSDL2 -lSDL2_image -lSDL2_ttf
INCLUDEPATH += /usr/include



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lSDL2main
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lSDL2main
else:unix: LIBS += -L$$PWD/lib/ -lSDL2main


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lSDL2_image
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lSDL2_image
else:unix: LIBS += -L$$PWD/lib/ -lSDL2_image

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lSDL2_ttf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lSDL2_ttf
else:unix: LIBS += -L$$PWD/lib/ -lSDL2_ttf

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lSDL2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lSDL2
else:unix: LIBS += -L$$PWD/lib/ -lSDL2

DEPENDPATH += $$PWD/include


INCLUDEPATH += $$PWD/include







