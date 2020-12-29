/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LIST_VIEW_H
#define QSK_LIST_VIEW_H

#include "QskScrollView.h"
#include "QskTextOptions.h"

class QSK_EXPORT QskListView : public QskScrollView
{
    Q_OBJECT

    Q_PROPERTY( bool alternatingRowColors READ alternatingRowColors
        WRITE setAlternatingRowColors NOTIFY alternatingRowColorsChanged FINAL )

    Q_PROPERTY( SelectionMode selectionMode READ selectionMode
        WRITE setSelectionMode NOTIFY selectionModeChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged FINAL )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged FINAL )

    Q_PROPERTY( bool preferredWidthFromColumns READ preferredWidthFromColumns
        WRITE setPreferredWidthFromColumns NOTIFY preferredWidthFromColumnsChanged() )

    using Inherited = QskScrollView;

  public:
    /*
        Everything, that can have a skin state, needs to be a QskSkinnable.
        Of course this is no option for the cells considering that we might
        have many, many of them.
        So for the moment we simply use Cell/Text and CellSelected/TextSelected
        as workaround until we found a solution that fits into the design.
        TODO ...
     */
    QSK_SUBCONTROLS( Cell, Text, CellSelected, TextSelected )

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

    void setAlternatingRowColors( bool );
    bool alternatingRowColors() const;

    void setSelectionMode( SelectionMode );
    SelectionMode selectionMode() const;

    void setTextOptions( const QskTextOptions& textOptions );
    QskTextOptions textOptions() const;

    Q_INVOKABLE int selectedRow() const;

    virtual int rowCount() const = 0;
    virtual int columnCount() const = 0;

    virtual qreal columnWidth( int col ) const = 0;
    virtual qreal rowHeight() const = 0;

    Q_INVOKABLE virtual QVariant valueAt( int row, int col ) const = 0;

#if 1
    virtual QskColorFilter graphicFilterAt( int row, int col ) const;
    virtual QskAspect::Subcontrol textSubControlAt( int row, int col ) const;
#endif

  public Q_SLOTS:
    void setSelectedRow( int row );

  Q_SIGNALS:
    void selectedRowChanged( int row );

    void selectionModeChanged();
    void alternatingRowColorsChanged();
    void preferredWidthFromColumnsChanged();
    void textOptionsChanged();

  protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void updateScrollableSize();

    void componentComplete() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
