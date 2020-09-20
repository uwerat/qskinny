#ifndef MAINCONTENT_H
#define MAINCONTENT_H

#include <QskLinearBox.h>

class QskGridBox;
class ShadowBox;
class ShadowedRectangle;

class ShadowPositioner : public QskControl
{
        Q_OBJECT
    public:
        ShadowPositioner( QQuickItem* parent );

        void updateLayout() override;
        void setGridBox( QskGridBox* gridBox );

    private:
        QskGridBox* m_gridBox;
        QVector<ShadowedRectangle*> m_rectangles;
};

class MainContent : public QskLinearBox
{
        Q_OBJECT

    public:
        MainContent( QQuickItem* parent );

    protected:
        void geometryChanged( const QRectF&, const QRectF& ) override;

    private:
        QList< QskLinearBox* > m_columns;
        ShadowPositioner* m_shadowPositioner;
};

#endif // MAINCONTENT_H
