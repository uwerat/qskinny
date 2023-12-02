#pragma once

#include <QskFocusIndicator.h>

class FocusIndicator : public QskFocusIndicator
{
    Q_OBJECT

    Q_PROPERTY( int duration READ duration
        WRITE setDuration NOTIFY durationChanged )

    using Inherited = QskFocusIndicator;

  public:
    FocusIndicator( QQuickItem* parent = nullptr );
    ~FocusIndicator() override;

    bool eventFilter( QObject*, QEvent* ) override;

    void setDuration( int ms );
    int duration() const;

  Q_SIGNALS:
    void durationChanged( int );

  protected:
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    void timerEvent( QTimerEvent* ) override;

  private:
    void resetTimer();
    void maybeEnable( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
