/******************************************************************************
 * QSkinny - Copyright (C) 2022 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_APPLICATION_VIEW_H
#define QSK_APPLICATION_VIEW_H

#include "QskLinearBox.h"

class QSK_EXPORT QskApplicationView : public QskLinearBox
{
    Q_OBJECT

    using Inherited = QskLinearBox;

  public:
    QskApplicationView( QQuickItem* parent = nullptr );
    ~QskApplicationView() override;

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
