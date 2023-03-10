/******************************************************************************
 * QSkinny - Copyright (C) 2022 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MAIN_VIEW_H
#define QSK_MAIN_VIEW_H

#include "QskLinearBox.h"

class QSK_EXPORT QskMainView : public QskLinearBox
{
    Q_OBJECT

    using Inherited = QskLinearBox;

  public:
    QskMainView( QQuickItem* parent = nullptr );
    ~QskMainView() override;

    QskControl* header() const;
    void setHeader( QskControl* );

    QskControl* body() const;
    void setBody( QskControl* );

    QskControl* footer() const;
    void setFooter( QskControl* );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
