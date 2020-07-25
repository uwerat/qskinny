/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOUNDED_VALUE_INPUT_H
#define QSK_BOUNDED_VALUE_INPUT_H

#include "QskControl.h"

class QskIntervalF;

class QSK_EXPORT QskBoundedValueInput : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged )
    Q_PROPERTY( QskIntervalF boundaries READ boundaries
                WRITE setBoundaries NOTIFY boundariesChanged )

    Q_PROPERTY( qreal value READ value WRITE setValue NOTIFY valueChanged )

    Q_PROPERTY( qreal stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged )
    Q_PROPERTY( int pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged )

    Q_PROPERTY( bool snap READ snap WRITE setSnap NOTIFY snapChanged )
    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged )

    using Inherited = QskControl;

  public:
    QSK_STATES( ReadOnly )

    QskBoundedValueInput( QQuickItem* parent = nullptr );
    ~QskBoundedValueInput() override;

    qreal minimum() const;
    qreal maximum() const;

    qreal boundaryLength() const;

    void setBoundaries( qreal min, qreal max );
    QskIntervalF boundaries() const;

    qreal value() const;

    // [0.0, 1.0]
    qreal valueAsRatio() const; 

    qreal stepSize() const;
    int pageSize() const;

    void setSnap( bool );
    bool snap() const;

    void setReadOnly( bool );
    bool isReadOnly() const;

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );

    void setMinimum( qreal );
    void setMaximum( qreal );
    void setBoundaries( const QskIntervalF& );

    void setStepSize( qreal );
    void setPageSize( int );

    void stepUp();
    void stepDown();
    void pageUp();
    void pageDown();

  Q_SIGNALS:
    void valueChanged( qreal );

    void minimumChanged( qreal );
    void maximumChanged( qreal );
    void boundariesChanged( const QskIntervalF&  );

    void stepSizeChanged( qreal );
    void pageSizeChanged( qreal );
    void snapChanged( bool );

    void readOnlyChanged( bool );

  protected:
    virtual qreal fixupValue( qreal value ) const;
    void keyPressEvent( QKeyEvent* event ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

    void componentComplete() override;

  private:
    void adjustBoundariesAndValue( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
