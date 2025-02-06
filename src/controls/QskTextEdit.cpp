/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextEdit.h"
#include "QskInternalMacros.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextedit_p.h>
#include <private/qquicktextedit_p_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextEdit, Text )
QSK_SUBCONTROL( QskTextEdit, TextPanel )

/*
    Other properties offered from QQuickTextEdit:

    - textMarginChanged
      should be the TextPanel::Padding

    - selectByKeyboardChanged(bool selectByKeyboard);
      Why is this one specific for QQuickTextEdit ) ?

    - tabStopDistanceChanged
      Why is this one specific for QQuickTextEdit ?
 */

namespace
{
    class QuickTextEdit final : public QQuickTextEdit
    {
        Q_OBJECT

        using Inherited = QQuickTextEdit;

      public:
        QuickTextEdit( QskTextEdit* );

        inline void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( HAlignment ) ( int( alignment ) & 0x0f ) );
            setVAlign( ( VAlignment ) ( int( alignment ) & 0xf0 ) );
        }

        Q_INVOKABLE void updateColors();
        Q_INVOKABLE void updateMetrics();
        Q_INVOKABLE void setEditing( bool );
        Q_INVOKABLE void handleEvent( QEvent* ev ) { event( ev ); }

      protected:

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        void geometryChange(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChange( newGeometry, oldGeometry );
            updateClip();
        }
#else
        void geometryChanged(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChanged( newGeometry, oldGeometry );
            updateClip();
        }
#endif

        void updateClip()
        {
            setClip( ( contentWidth() > width() ) ||
                ( contentHeight() > height() ) );
        }

        QSGNode* updatePaintNode(
            QSGNode* oldNode, UpdatePaintNodeData* data ) override
        {
            updateColors();
            return Inherited::updatePaintNode( oldNode, data );
        }
    };

    QuickTextEdit::QuickTextEdit( QskTextEdit* textField )
        : QQuickTextEdit( textField )
    {
        classBegin();

        setActiveFocusOnTab( false );
        setFlag( ItemAcceptsInputMethod, false );
        setFocusOnPress( false );
        setSelectByMouse( true );

        componentComplete();

        connect( this, &QuickTextEdit::contentSizeChanged,
            this, &QuickTextEdit::updateClip );
    }

    void QuickTextEdit::setEditing( bool on )
    {
        Q_ASSERT( focusOnPress() == false );

        QFocusEvent event( on ? QEvent::FocusIn : QEvent::FocusOut );
        QQuickTextEditPrivate::get( this )->handleFocusEvent( &event );
    }

    void QuickTextEdit::updateMetrics()
    {
        auto textEdit = static_cast< const QskTextEdit* >( parentItem() );

        setAlignment( textEdit->alignment() );
        setFont( textEdit->font() );
    }

    void QuickTextEdit::updateColors()
    {
        using Q = QskTextEdit;

        auto input = static_cast< const Q* >( parentItem() );

        setColor( input->color( Q::Text ) );

        const auto state = QskTextEdit::Selected;

        setSelectionColor( input->color( Q::TextPanel | state ) );
        setSelectedTextColor( input->color( Q::Text | state ) );
    }
}

class QskTextEdit::PrivateData
{
  public:
    QuickTextEdit* wrappedEdit;
};

QskTextEdit::QskTextEdit( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->wrappedEdit = new QuickTextEdit( this );
    auto wrappedEdit = m_data->wrappedEdit;

    setAcceptedMouseButtons( wrappedEdit->acceptedMouseButtons() );
    wrappedEdit->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

    setup( wrappedEdit );

    connect( wrappedEdit, &QQuickTextEdit::lineCountChanged,
        this, [this]() { Q_EMIT lineCountChanged( lineCount() ); } );

    connect( wrappedEdit, &QQuickTextEdit::linkActivated,
        this, &QskTextEdit::linkActivated );

    connect( wrappedEdit, &QQuickTextEdit::linkHovered,
        this, &QskTextEdit::linkHovered );

    connect( wrappedEdit, &QQuickTextEdit::linkActivated,
        this, &QskTextEdit::linkActivated );
}

QskTextEdit::~QskTextEdit()
{
}

QskAspect::Subcontrol QskTextEdit::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Inherited::Text )
        return Text;

    if ( subControl == Inherited::TextPanel )
        return TextPanel;

    return Inherited::substitutedSubcontrol( subControl );
}

QUrl QskTextEdit::baseUrl() const
{
    return m_data->wrappedEdit->baseUrl();
}

void QskTextEdit::setBaseUrl( const QUrl& url )
{
    if ( url != m_data->wrappedEdit->baseUrl() )
    {
        m_data->wrappedEdit->setBaseUrl( url );
        Q_EMIT baseUrlChanged( url );
    }
}

void QskTextEdit::resetBaseUrl()
{
    m_data->wrappedEdit->resetBaseUrl();
}

void QskTextEdit::setTextFormat( QskTextOptions::TextFormat format )
{
    if ( format != textFormat() )
    {
        m_data->wrappedEdit->setTextFormat(
            static_cast< QQuickTextEdit::TextFormat >( format ) );

        Q_EMIT textFormatChanged( format );
    }
}

QskTextOptions::TextFormat QskTextEdit::textFormat() const
{
    return static_cast< QskTextOptions::TextFormat >(
        m_data->wrappedEdit->textFormat() );
}

int QskTextEdit::lineCount() const
{
    return m_data->wrappedEdit->lineCount();
}

int QskTextEdit::tabStopDistance() const
{
    return m_data->wrappedEdit->tabStopDistance();
}

void QskTextEdit::setTabStopDistance( qreal distance )
{
    // QTextOption !
    m_data->wrappedEdit->setTabStopDistance( distance );
}

#include "QskTextEdit.moc"
#include "moc_QskTextEdit.cpp"
