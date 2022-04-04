#ifndef QSK_CHECK_BOX_H
#define QSK_CHECK_BOX_H

#include "QskAbstractButton.h"

class QSK_EXPORT QskCheckBox : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( Qt::CheckState checkState READ checkState
        WRITE setCheckState NOTIFY checkStateChanged FINAL )
    Q_PROPERTY( bool isTriState READ isTriState
        WRITE setTriState NOTIFY isTriStateChanged FINAL )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Box, Tick )
    QSK_STATES( PartiallyChecked )

    QskCheckBox( QQuickItem* parent = nullptr );
    ~QskCheckBox() override;

    Qt::CheckState checkState() const;
    bool isTriState() const;
    bool isCheckable() const override final;

    void addToGroup( QskCheckBox* groupItem );
    void removeFromGroup( QskCheckBox* groupItem );

  public Q_SLOTS:
    void setCheckState( Qt::CheckState );
    void setTriState( bool triState = true );

  Q_SIGNALS:
    void checkStateChanged( Qt::CheckState );
    void isTriStateChanged( bool );
    void removeFromAllGroupsRequested();

  private:
    void setCheckStateInternal( Qt::CheckState );
    void updated();

    struct PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif // QSK_CHECK_BOX_H
