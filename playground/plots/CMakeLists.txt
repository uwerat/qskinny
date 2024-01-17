############################################################################
# QSkinny - Copyright (C) The authors
# This file may be used under the terms of the 3-clause BSD License
############################################################################

list(APPEND HEADERS QskPlotView.h QskPlotItem.h QskPlotViewSkinlet.h QskPlotNamespace.h)
list(APPEND SOURCES QskPlotView.cpp QskPlotItem.cpp QskPlotViewSkinlet.cpp)

list(APPEND HEADERS QskPlotGrid.h QskPlotGridSkinlet.h)
list(APPEND SOURCES QskPlotGrid.cpp QskPlotGridSkinlet.cpp)

list(APPEND HEADERS QskPlotCurveData.h QskPlotCurve.h QskPlotCurveSkinlet.h )
list(APPEND SOURCES QskPlotCurveData.cpp QskPlotCurve.cpp QskPlotCurveSkinlet.cpp)

list(APPEND HEADERS QskPlotCorridorData.h QskPlotCorridor.h QskPlotCorridorSkinlet.h )
list(APPEND SOURCES QskPlotCorridorData.cpp QskPlotCorridor.cpp QskPlotCorridorSkinlet.cpp)

list(APPEND HEADERS PlotCursor.h PlotCursorSkinlet.h )
list(APPEND SOURCES PlotCursor.cpp PlotCursorSkinlet.cpp)

qsk_add_example(plots ${SOURCES} ${HEADERS}
    PlotSkin.h PlotSkin.cpp Plot.h Plot.cpp main.cpp )
