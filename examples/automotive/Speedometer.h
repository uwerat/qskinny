#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <QskControl.h>

class Speedometer : public QskControl
{
    Q_OBJECT

public:
    QSK_SUBCONTROLS( Panel, Labels, Needle )

    Speedometer( QQuickItem* parent = nullptr );

    float value() const;
    void setValue( float value ); // angle; should be within a set range

    float startAngle() const;
    void setStartAngle( float startAngle );

    float endAngle() const;
    void setEndAngle( float endAngle );

    QVector< QString > labels() const;
    void setLabels( const QVector< QString >& labels );

private:
    float m_value;
    float m_startAngle;
    float m_endAngle;
    float m_labelsStep;
    QVector< QString > m_labels;
};

#endif // SPEEDOMETER_H
