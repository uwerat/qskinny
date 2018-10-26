QSK_ROOT = $${PWD}/..

INCLUDEPATH *= $${QSK_ROOT}/qmlexport
DEPENDPATH  *= $${QSK_ROOT}/qmlexport

qskAddLibrary($${QSK_OUT_ROOT}/lib, qskqmlexport)

