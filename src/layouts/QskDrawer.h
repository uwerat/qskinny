#include "QskPopup.h"

#ifndef QSK_DRAWER_H
#define QSK_DRAWER_H

class QSK_EXPORT QskDrawer : public QskPopup
{
    Q_OBJECT

    using Inherited = QskPopup;

    Q_PROPERTY( Qt::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged )
    Q_PROPERTY( Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged )

  public:
    QSK_SUBCONTROLS( Panel, Overlay )
    QskDrawer( QQuickItem* parentItem = nullptr );
    ~QskDrawer() override;

    Qt::Edge edge() const;
    Qt::Alignment alignment() const;

    void updateLayout() override;

    void setContent( QskControl* t );
    void setEdge( Qt::Edge edge );
    void setAlignment( Qt::Alignment aligmnent );

  protected:
    void aboutToShow() override;

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );
    void alignmentChanged( Qt::Alignment );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
