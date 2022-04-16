#!/bin/sh
LD_LIBRARY_PATH=/home/uwe/qskinny/config/qsk1/Qt/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=/home/uwe/qskinny/config/qsk1/Qt/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec "$@"
