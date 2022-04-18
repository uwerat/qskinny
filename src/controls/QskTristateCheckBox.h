#ifndef QSK_TRISTATE_CHECK_BOX_H
#define QSK_TRISTATE_CHECK_BOX_H

#include "QskCheckBox.h"

class QSK_EXPORT QskTristateCheckBox : public QskCheckBox
{
    Q_OBJECT

    Q_PROPERTY( Qt::CheckState checkState READ checkState
        WRITE setCheckState NOTIFY checkStateChanged FINAL )

    using Inherited = QskCheckBox;

  public:
    QSK_STATES( PartiallyChecked )

    QskTristateCheckBox( QQuickItem* parent = nullptr );
    ~QskTristateCheckBox() override;

    Qt::CheckState checkState() const;

  public Q_SLOTS:
    void setCheckState( Qt::CheckState );

  Q_SIGNALS:
    void checkStateChanged( Qt::CheckState );
    void tristateChanged( bool );

  private:
    void setCheckStateInternal( Qt::CheckState );
    void updated();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
