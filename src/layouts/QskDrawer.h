#include "QskGlobal.h"
#include "QskPopup.h"

#include <memory>
#include <qnamespace.h>
#include <qobjectdefs.h>

#ifndef QSK_DRAWER_H
#define QSK_DRAWER_H

class QSK_EXPORT QskDrawer : public QskPopup
{
    Q_OBJECT

    using Inherited = QskPopup;

    Q_PROPERTY( Qt::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged )

 public:
    QSK_SUBCONTROLS( DasPanel )
    QskDrawer( QQuickItem* parentItem = nullptr );
    ~QskDrawer() override;

    Qt::Edge edge() const;

    void updateLayout() override;

    void setContent( QskControl* t );
    void setEdge( Qt::Edge edge );

 protected:
    void aboutToShow() override;
  
  Q_SIGNALS:
    void edgeChanged( Qt::Edge );

 private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
