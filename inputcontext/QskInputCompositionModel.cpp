/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputCompositionModel.h"

QskInputCompositionModel::QskInputCompositionModel(
        Attributes attributes, QObject* parent ):
    QObject( parent ),
    m_attributes( attributes )
{
}

QskInputCompositionModel::~QskInputCompositionModel()
{
}

QskInputCompositionModel::Attributes QskInputCompositionModel::attributes() const
{
    return m_attributes;
}

#include "moc_QskInputCompositionModel.cpp"
