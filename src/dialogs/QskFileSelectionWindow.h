/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FILE_SELECTION_WINDODW_H
#define QSK_FILE_SELECTION_WINDODW_H

#include "QskWindowOrSubWindow.h"

#include <QDir>

template< typename W >
class QskFileSelectionWindow : public QskWindowOrSubWindow< W >
{
    using Inherited = QskWindowOrSubWindow< W >;

  public:

    QskFileSelectionWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction,
        const QString& directory, QDir::Filters filters );
    ~QskFileSelectionWindow();

    QString selectedPath() const;

  private:
    void setupHeader( QQuickItem* parentItem );

    static QStringList splitPath( const QString& path );

    void updateHeader( const QString& path );

    void setupFileSystemView( const QString& directory, QDir::Filters filters, QQuickItem* parentItem );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData >m_data;
};

#endif
