/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MENU_BUTTON_H
#define QSK_MENU_BUTTON_H

#include "QskPushButton.h"

class QskMenu;
class QskLabelData;

class QSK_EXPORT QskMenuButton : public QskPushButton
{
    Q_OBJECT

    Q_PROPERTY( QVector< QskLabelData > options READ options
        WRITE setOptions NOTIFY optionsChanged )

    Q_PROPERTY( int optionsCount READ optionsCount )
    Q_PROPERTY( int triggeredIndex READ triggeredIndex NOTIFY triggered )
    Q_PROPERTY( QString triggeredText READ triggeredText NOTIFY triggered )

  public:
    QskMenuButton( QQuickItem* parent = nullptr );
    QskMenuButton( const QString& text, QQuickItem* parent = nullptr );

    ~QskMenuButton() override;

    int addOption( const QString&, const QString& );
    int addOption( const QUrl&, const QString& );
    int addOption( const QskLabelData& );
    void addSeparator();

    void setOptions( const QVector< QskLabelData >& );
    void setOptions( const QStringList& );

    QVector< QskLabelData > options() const;
    QskLabelData optionAt( int ) const;

    int optionsCount() const;

    const QskMenu* menu() const;

    int triggeredIndex() const;
    QString triggeredText() const;

  public Q_SLOTS:
    void setStartIndex( int );
    void clear();

  Q_SIGNALS:
    void triggered( int index );
    void optionsChanged();

  protected:
    virtual void openMenu();

  private:
    void updateTriggeredIndex( int );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
