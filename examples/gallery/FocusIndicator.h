#pragma once

#include <QskFocusIndicator.h>

/*
    A focus indicator, that becomes temporarily visible when using the keyboard
    It is intended to move the code to QskFocusIndicator later.
 */
class FocusIndicator : public QskFocusIndicator
{
    Q_OBJECT

    Q_PROPERTY( bool exposed READ isExposed
        WRITE setExposed NOTIFY exposedChanged )

    Q_PROPERTY( int exposedTimeout READ exposedTimeout
        WRITE setExposedTimeout NOTIFY exposedTimeoutChanged )

    using Inherited = QskFocusIndicator;

  public:
    QSK_STATES( Concealed )

    FocusIndicator( QQuickItem* parent = nullptr );
    ~FocusIndicator() override;

    bool isExposed() const;
    bool isConcealed() const;

    bool eventFilter( QObject*, QEvent* ) override;

    void setExposedTimeout( int ms );
    int exposedTimeout() const;

  public Q_SLOTS:
    void setExposed( bool = true );
    void setConcealed( bool = true );

  Q_SIGNALS:
    void exposedChanged( bool );
    void exposedTimeoutChanged( int );

  protected:
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    void timerEvent( QTimerEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void FocusIndicator::setConcealed( bool on )
{
    setExposed( !on );
}

inline bool FocusIndicator::isConcealed() const
{
    return !isExposed();
}
