/******************************************************************************
 * Qsk Controls
 * Copyright (C) 2016 Uwe Rathmann
 *
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskInputPanelSkinlet.h"
#include "QskInputPanel.h"

#include "QskAspect.h"
#include "QskSkin.h"
#include "QskTextOptions.h"
#include "QskTextNode.h"
#include "QskBoxNode.h"

static constexpr const QSGNode::Flag IsSubtreeBlocked =
    static_cast< QSGNode::Flag >( 0x100000 );
static constexpr const QSGNode::Flag KeyFrameNode =
    static_cast< QSGNode::Flag >( 0x200000 );
static constexpr const QSGNode::Flag KeyGlyphNode =
    static_cast< QSGNode::Flag >( 0x400000 );

namespace
{
    class KeySkinnable: public QskSkinnable
    {
    public:
        KeySkinnable( QskInputPanel* panel ):
            m_panel( panel )
        {
        }

        void setKey( Qt::Key key )
        {
            setSkinStateFlag( QskInputPanel::Pressed, key & Qt::ShiftModifier );
            setSkinStateFlag( QskInputPanel::Checked, key & Qt::ControlModifier );
            setSkinStateFlag( QskInputPanel::Focused, key & Qt::MetaModifier );
        }

        const QMetaObject* metaObject() const override final
        {
            // Use the parent skinlet
            return &QskInputPanelSkinlet::staticMetaObject;
        }

    protected:
        virtual const QskSkinlet* effectiveSkinlet() const override final
        {
            return m_panel->effectiveSkinlet();
        }

    private:
        virtual QskControl* owningControl() const override final
        {
            return const_cast< QskInputPanel* >( m_panel );
        }

    private:
        QskInputPanel* m_panel;
    };

    class FrameNode final : public QskBoxNode, public KeySkinnable
    {
    public:
        FrameNode( QskInputPanel* panel ):
            KeySkinnable( panel )
        {
            setFlag( KeyFrameNode );
        }
    };

    class TextNode final : public QskTextNode, public KeySkinnable
    {
    public:
        TextNode( QskInputPanel* panel ):
            KeySkinnable( panel )
        {
            setFlag( KeyGlyphNode );
        }
    };

    class InputPanelNode final : public QskBoxNode
    {
    public:
        InputPanelNode()
        {
        }

        virtual bool isSubtreeBlocked() const override final
        {
            return flags() & IsSubtreeBlocked;
        }

        using Row = QSGNode*[ QskInputPanel::KeyCount ];
        Row frames[ QskInputPanel::RowCount ] = {};
        Row glyphs[ QskInputPanel::RowCount ] = {};
    };
}

static_assert( QskInputPanel::RowCount * QskInputPanel::KeyCount < 255,
    "The number of keys must fit into an unsigned byte." );

QskInputPanelSkinlet::QskInputPanelSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { Panel0, Panel1, Panel2 } );
}

QskInputPanelSkinlet::~QskInputPanelSkinlet()
{
}

QSGNode* QskInputPanelSkinlet::updateSubNode(
    const QskSkinnable* control, quint8 nodeRole, QSGNode* node ) const
{
    auto inputPanel = static_cast< const QskInputPanel* >( control );
    const bool blockSubtree = inputPanel->mode() != nodeRole;

    auto panelNode = static_cast< InputPanelNode* >( node );
    if ( panelNode && panelNode->isSubtreeBlocked() != blockSubtree )
    {
        panelNode->setFlag( IsSubtreeBlocked, blockSubtree );
        panelNode->markDirty( QSGNode::DirtySubtreeBlocked );
    }

    if ( !blockSubtree )
        node = updatePanelNode( inputPanel, panelNode );

    return node;
}

QSGNode* QskInputPanelSkinlet::updatePanelNode(
    const QskInputPanel* panel, QSGNode* node ) const
{
    auto panelNode = static_cast< InputPanelNode* >( node );
    if ( panelNode == nullptr )
        panelNode = new InputPanelNode;

    const auto contentsRect = panel->keyboardRect();

    auto& panelKeyData = panel->keyData();
    for ( const auto& keyRow : panelKeyData )
    {
        const auto rowIndex = &keyRow - panelKeyData;
        auto& frames = panelNode->frames[ rowIndex ];
        auto& glyphs = panelNode->glyphs[ rowIndex ];

        for ( const auto& keyData : keyRow )
        {
            const auto colIndex = &keyData - keyRow;
            auto& frame = frames[ colIndex ];
            auto& glyph = glyphs[ colIndex ];

            if ( !keyData.key || keyData.key == Qt::Key_unknown )
            {
                delete frame;
                frame = nullptr;

                delete glyph;
                glyph = nullptr;

                continue;
            }

            const qreal sx = contentsRect.size().width();
            const qreal sy = contentsRect.size().height();

            const QRectF keyRect(
                contentsRect.x() + keyData.rect.x() * sx,
                contentsRect.y() + keyData.rect.y() * sy,
                keyData.rect.width() * sx,
                keyData.rect.height() * sy );

            frame = updateKeyFrameNode( panel, frame, keyRect, keyData.key );
            if ( frame->parent() != panelNode )
                panelNode->appendChildNode( frame );

            glyph = updateKeyGlyphNode( panel, glyph, keyRect, keyData.key );
            if ( glyph->parent() != panelNode )
                panelNode->appendChildNode( glyph );
        }
    }

    updateBoxNode( panel, panelNode, contentsRect, QskInputPanel::Panel );

    return panelNode;
}

QSGNode* QskInputPanelSkinlet::updateKeyFrameNode(
    const QskInputPanel* panel, QSGNode* node,
    const QRectF& rect, Qt::Key key ) const
{
    auto frameNode = static_cast< FrameNode* >( node );
    if ( frameNode == nullptr )
        frameNode = new FrameNode( const_cast< QskInputPanel* >( panel ) );

    frameNode->setKey( key );
    updateBoxNode( frameNode, frameNode, rect, QskInputPanel::KeyPanel );

    return frameNode;
}

QSGNode* QskInputPanelSkinlet::updateKeyGlyphNode(
    const QskInputPanel* panel, QSGNode* node,
    const QRectF& rect, Qt::Key key ) const
{
    auto textNode = static_cast< TextNode* >( node );
    if ( textNode == nullptr )
        textNode = new TextNode( const_cast< QskInputPanel* >( panel ) );

    textNode->setKey( key );

    QskTextOptions options;
    options.setFontSizeMode( QskTextOptions::VerticalFit );

    const auto alignment = textNode->flagHint< Qt::Alignment >(
        QskInputPanel::KeyGlyph | QskAspect::Alignment, Qt::AlignCenter );

    return updateTextNode( panel, textNode, rect, alignment,
        panel->textForKey( key ), options, QskInputPanel::KeyGlyph );
}

#include "moc_QskInputPanelSkinlet.cpp"
