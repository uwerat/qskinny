/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LIST_VIEW_H
#define QSK_LIST_VIEW_H

#include "QskScrollView.h"
#include "QskTextOptions.h"

class QSK_EXPORT QskListView : public QskScrollView
{
    Q_OBJECT

    Q_PROPERTY( SelectionMode selectionMode READ selectionMode
        WRITE setSelectionMode NOTIFY selectionModeChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged USER true FINAL )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions RESET resetTextOptions
        NOTIFY textOptionsChanged FINAL )

    Q_PROPERTY( bool preferredWidthFromColumns READ preferredWidthFromColumns
        WRITE setPreferredWidthFromColumns NOTIFY preferredWidthFromColumnsChanged() )

    using Inherited = QskScrollView;

  public:
    QSK_SUBCONTROLS( Cell, Text, Graphic )
    QSK_STATES( Selected )

    enum SelectionMode
    {
        NoSelection,
        SingleSelection,
        MultiSelection // not implemented yet
    };
    Q_ENUM( SelectionMode )

    QskListView( QQuickItem* parent = nullptr );
    ~QskListView() override;

    void setPreferredWidthFromColumns( bool );
    bool preferredWidthFromColumns() const;

    void setSelectionMode( SelectionMode );
    SelectionMode selectionMode() const;

    void setTextOptions( const QskTextOptions& textOptions );
    void resetTextOptions();
    QskTextOptions textOptions() const;

    Q_INVOKABLE int selectedRow() const;
    QskAspect::States rowStates( int ) const;

    virtual int rowCount() const = 0;
    virtual int columnCount() const = 0;

    virtual qreal columnWidth( int col ) const = 0;
    virtual qreal rowHeight() const = 0;

    Q_INVOKABLE virtual QVariant valueAt( int row, int col ) const = 0;

    QRectF focusIndicatorRect() const override;

  public Q_SLOTS:
    void setSelectedRow( int row );

  Q_SIGNALS:
    void selectedRowChanged( int row );

    void selectionModeChanged();
    void preferredWidthFromColumnsChanged();
    void textOptionsChanged();

  protected:
    void changeEvent( QEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    virtual QPointF scrollOffset( const QWheelEvent* ) const override;
#endif

    void updateScrollableSize();

    void componentComplete() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
