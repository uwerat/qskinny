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

    Q_ENUM( Policy )

    // a.k.a QMessageBox::StandardButton or QPlatformDialogHelper::StandardButton
    enum Action
    {
        NoAction           = 0,
        Ok                 = 1 << 10,
        Save               = 1 << 11,
        SaveAll            = 1 << 12,
        Open               = 1 << 13,
        Yes                = 1 << 14,
        YesToAll           = 1 << 15,
        No                 = 1 << 16,
        NoToAll            = 1 << 17,
        Abort              = 1 << 18,
        Retry              = 1 << 19,
        Ignore             = 1 << 20,
        Close              = 1 << 21,
        Cancel             = 1 << 22,
        Discard            = 1 << 23,
        Help               = 1 << 24,
        Apply              = 1 << 25,
        Reset              = 1 << 26,
        RestoreDefaults    = 1 << 27
    };

    Q_ENUM( Action )
    Q_DECLARE_FLAGS( Actions, Action )

    // a.k.a QMessageBox::ButtonRole
    enum ActionRole
    {
        InvalidRole = -1,

        AcceptRole,
        RejectRole,
        DestructiveRole,
        UserRole,
        HelpRole,
        YesRole,
        NoRole,
        ResetRole,
        ApplyRole,

        NActionRoles
    };

    Q_ENUM( ActionRole )

    // for building the mask in QskSkin::dialogButtonLayout
    enum ButtonLayoutFlag
    {
        // from QPlatformDialogHelper::ButtonRole
        ActionMask      = 0x0FFFFFFF,

        AlternateRole   = 1 << 28,
        Stretch         = 1 << 29,
        Reverse         = 1 << 30
    };

    enum DialogCode
    {
        Rejected = 0,
        Accepted
    };

    Q_ENUM( DialogCode )

    static QskDialog* instance();

    void setPolicy( Policy );
    Policy policy() const;

    Q_INVOKABLE void setTransientParent( QWindow* );
    Q_INVOKABLE QWindow* transientParent() const;

    Q_INVOKABLE Action message(
        const QString& title, const QString& text, int symbolType,
        Actions actions = Ok, Action defaultAction = NoAction ) const;

    Q_INVOKABLE Action information(
        const QString& title, const QString& text,
        Actions actions = Ok, Action defaultAction = NoAction ) const;

    Q_INVOKABLE Action warning(
        const QString& title, const QString& text,
        Actions actions = Ok, Action defaultAction = NoAction ) const;

    Q_INVOKABLE Action critical(
        const QString& title, const QString& text,
        Actions actions = Ok, Action defaultAction = NoAction ) const;

    Q_INVOKABLE Action question(
        const QString& title, const QString& text,
        Actions actions = Actions( Yes | No ),
        Action defaultAction = NoAction ) const;

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

Q_DECLARE_METATYPE( QskDialog::Action )
Q_DECLARE_METATYPE( QskDialog::Actions )
Q_DECLARE_OPERATORS_FOR_FLAGS( QskDialog::Actions )

#endif
