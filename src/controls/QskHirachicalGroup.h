#ifndef QSK_HIRACHICAL_GROUP
#define QSK_HIRACHICAL_GROUP

#include <qobject.h>
#include <memory>

#include "QskGlobal.h"

class QskTristateCheckBox;
class QskCheckBox;

class QSK_EXPORT QskHirachicalGroup : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

  public:
    QskHirachicalGroup( QObject* parent = nullptr );
    QskHirachicalGroup( QskTristateCheckBox* head );
    ~QskHirachicalGroup() override;

    QskTristateCheckBox* head() const;

  public Q_SLOTS:
    void setHead( QskTristateCheckBox* head );
    void add( QskCheckBox* newMember );
    bool remove( QskCheckBox* removal );

  Q_SIGNALS:
    void headChanged( QskTristateCheckBox* newHead );
    void added( QskCheckBox* added );
    void removed( QskCheckBox* removed );

  private:
    void update();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
