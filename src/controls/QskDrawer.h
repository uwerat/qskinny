#ifndef QSK_DRAWER_H
#define QSK_DRAWER_H

#include "QskPopup.h"
#include <qnamespace.h>

class QSK_EXPORT QskDrawer : public QskPopup
{
    Q_OBJECT

    using Inherited = QskPopup;

    Q_PROPERTY( Qt::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged )

  public:
    QSK_SUBCONTROLS( Panel, Overlay )

    QskDrawer( QQuickItem* = nullptr );
    ~QskDrawer() override;

    void setEdge( Qt::Edge );
    Qt::Edge edge() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;
    void updateLayout() override;

    void setContent( QskControl* );

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );

  protected:
    void aboutToShow() override;
    void itemChange( ItemChange, const ItemChangeData& ) override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void startFading( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
