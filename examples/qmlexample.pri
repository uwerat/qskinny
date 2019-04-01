QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

INCLUDEPATH *= $${QSK_ROOT}/qmlexport
DEPENDPATH  *= $${QSK_ROOT}/qmlexport

qskAddLibrary($${QSK_OUT_ROOT}/lib, qskqmlexport)
