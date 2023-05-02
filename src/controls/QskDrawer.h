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

    QskDrawer( QQuickItem* parentItem = nullptr );
    ~QskDrawer() override;

    void setEdge( Qt::Edge edge );
    Qt::Edge edge() const;

    void updateLayout() override;

    void setContent( QskControl* t );

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );

  protected:
    void aboutToShow() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
