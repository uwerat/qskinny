/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextNode.h"
#include "QskTextOptions.h"

#include <QFont>
#include <QString>

QskTextNode::QskTextNode():
    m_hash( 0 )
{
}

QskTextNode::~QskTextNode()
{
}

bool QskTextNode::setTextData(
    const QString& text, const QSizeF& size, const QFont& font,
    const QskTextOptions& options, Qt::Alignment alignment, Qsk::TextStyle textStyle,
    const QColor& textColor, const QColor& styleColor, const QColor& linkColor ) 
{
    uint hash;

    hash = qHash( text, 0 );
    hash = qHash( font, hash );
    hash = qHash( options, hash );
    hash = qHash( alignment, hash );
    hash = qHash( textStyle, hash );
    hash = qHash( textColor.rgba(), hash );
    hash = qHash( styleColor.rgba(), hash );
    hash = qHash( linkColor.rgba(), hash );
    hash = qHashBits( &size, sizeof( QSizeF ), hash );

    if ( hash == m_hash ) 
        return false;

    m_hash = hash;
    return true;
}
