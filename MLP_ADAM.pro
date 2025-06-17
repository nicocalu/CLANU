TEMPLATE = subdirs

CONFIG += ordered c++20

SUBDIRS = libmlp  libread  \    
    irm_train_sgd \
    irm_test


INCLUDEPATH += $$PWD/libmlp
DEPENDPATH += $$PWD/libmlp

INCLUDEPATH += $$PWD/libread
DEPENDPATH += $$PWD/libread

irm_train_sgd.subdir  = irm/irm_train_sgd
irm_train_sgd.depends  = libmlp libread

irm_test.subdir  = irm/irm_test
irm_test.depends  = libmlp libread




win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libmlp/release/liblibmlp.a $$OUT_PWD/libread/release/liblibread.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libmlp/debug/liblibmlpd.a $$OUT_PWD/libread/debug/liblibreadd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libmlp/release/libmlp.lib $$OUT_PWD/libread/release/libread.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libmlp/debug/libmlpd.lib $$OUT_PWD/libread/debug/libreadd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/libmlp/liblibmlp.a $$OUT_PWD/libread/liblibread.a


