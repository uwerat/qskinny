/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_H
#define QSK_DIALOG_H 1

#include "QskGlobal.h"

#include <qobject.h>
#include <memory>

class QString;
class QStringList;
class QWindow;

#if defined( qskDialog )
#undef qskDialog
#endif

#define qskDialog QskDialog::instance()

class QSK_EXPORT QskDialog : public QObject
{
    Q_OBJECT

    Q_PROPERTY( Policy policy READ policy
        WRITE setPolicy NOTIFY policyChanged )

    Q_PROPERTY( QWindow* transientParent READ transientParent
        WRITE setTransientParent NOTIFY transientParentChanged )

  public:
    enum Policy
    {
        EmbeddedBox,
        EmbeddedWindow, // not yet implemented, do we need it ?
        TopLevelWindow
    };

    enum StandardButton
    {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,
    };

    enum ButtonRole
    {
        InvalidRole = -1,

        AcceptRole,
        RejectRole,
        DestructiveRole,
        ActionRole,
        HelpRole,
        YesRole,
        NoRole,
        ResetRole,
        ApplyRole,

        NButtonRoles
    };

    enum DialogCode
    {
        Rejected = 0,
        Accepted
    };

    Q_ENUM( Policy )
    Q_ENUM( DialogCode )
    Q_ENUM( ButtonRole )
    Q_ENUM( StandardButton )

    Q_DECLARE_FLAGS( StandardButtons, StandardButton )

    static QskDialog* instance();

    Q_INVOKABLE void setPolicy( Policy );
    Q_INVOKABLE Policy policy() const;

    Q_INVOKABLE void setTransientParent( QWindow* );
    Q_INVOKABLE QWindow* transientParent() const;

    Q_INVOKABLE StandardButton message(
        const QString& title, const QString& text,
        int symbolType, StandardButtons buttons = Ok,
        StandardButton defaultButton = NoButton
    ) const;

    Q_INVOKABLE StandardButton information(
        const QString& title, const QString& text,
        StandardButtons buttons = Ok,
        StandardButton defaultButton = NoButton
    ) const;

    Q_INVOKABLE StandardButton warning(
        const QString& title, const QString& text,
        StandardButtons buttons = Ok, StandardButton = NoButton ) const;

    Q_INVOKABLE StandardButton critical(
        const QString& title, const QString& text,
        StandardButtons buttons = Ok, StandardButton = NoButton ) const;

    Q_INVOKABLE StandardButton question(
        const QString& title, const QString& text,
        StandardButtons = StandardButtons( Yes | No ), StandardButton = NoButton ) const;

    Q_INVOKABLE QString select(
        const QString& title, const QString& text,
        const QStringList& entries, int selectedRow = 0 ) const;

  Q_SIGNALS:
    void transientParentChanged();
    void policyChanged();

  private:
    QskDialog();
    ~QskDialog() override;

    static QskDialog* s_instance;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_METATYPE( QskDialog::StandardButton )
Q_DECLARE_METATYPE( QskDialog::StandardButtons )
Q_DECLARE_OPERATORS_FOR_FLAGS( QskDialog::StandardButtons )

#endif
