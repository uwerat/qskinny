#ifndef ROUNDEDICON_H
#define ROUNDEDICON_H

#include <QskBox.h>
#include <QskGradient.h>

class QskGraphicLabel;

class RoundedIcon : public QskBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )
        QSK_STATES( Bright ) // just to differentiate between orange and purple

        RoundedIcon( const QString& iconName, bool isBright, QQuickItem* parent = nullptr );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }

    protected:
        void updateLayout() override;
        virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    private:
        QString m_iconName;
        QskGraphicLabel* m_graphicLabel = nullptr;
};

#endif // ROUNDEDICON_H
