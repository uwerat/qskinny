#pragma once

#include <QskFocusIndicator.h>

/*
    A focus indicator, that becomes temporarily visible when using the keyboard
    It is intended to move the code to QskFocusIndicator later.
 */
class FocusIndicator : public QskFocusIndicator
{
    Q_OBJECT

    Q_PROPERTY( bool concealed READ isConcealed
        WRITE setConcealed NOTIFY concealedChanged )

    using Inherited = QskFocusIndicator;

  public:
    QSK_STATES( Concealed )

    FocusIndicator( QQuickItem* parent = nullptr );
    ~FocusIndicator() override;

    bool isConcealed() const;

    bool eventFilter( QObject*, QEvent* ) override;

  public Q_SLOTS:
    void setConcealed( bool );

  Q_SIGNALS:
    void concealedChanged( bool );

  protected:
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    void timerEvent( QTimerEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
