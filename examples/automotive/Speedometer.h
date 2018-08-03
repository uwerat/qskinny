#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <QskRangeControl.h>

class Speedometer : public QskRangeControl
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, Labels, NeedleHead, Needle )

    Speedometer( QQuickItem* parent = nullptr );

    QVector< QString > labels() const;
    void setLabels( const QVector< QString >& labels );

  private:
    QVector< QString > m_labels;
};

#endif // SPEEDOMETER_H
