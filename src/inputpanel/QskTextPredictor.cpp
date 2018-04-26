/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextPredictor.h"

QskTextPredictor::QskTextPredictor(
        Attributes attributes, QObject* parent ):
    QObject( parent ),
    m_attributes( attributes )
{
}

QskTextPredictor::~QskTextPredictor()
{
}

QskTextPredictor::Attributes QskTextPredictor::attributes() const
{
    return m_attributes;
}

#include "moc_QskTextPredictor.cpp"
