#include "QskRadioBoxSkinlet.h"

#include "QskAspect.h"
#include "QskRadioBox.h"

#include "QskStandardSymbol.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskFunctions.h"
#include "QskSkin.h"
#include <type_traits>

namespace {
    using Q = QskRadioBox;
};

QskRadioBoxSkinlet::QskRadioBoxSkinlet( QskSkin* )
{
    setNodeRoles( { PanelRole, ButtonRole, SymbolRole, TextRole, RippleRole } );
}

QskRadioBoxSkinlet::~QskRadioBoxSkinlet()
{
}

QRectF QskRadioBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol) const
{
    auto radio = static_cast<const QskRadioBox*>( skinnable );

    if( subcontrol == Q::Ripple ) {
	return buttonRect(radio, Q::Ripple, contentsRect, radio->focusedIndex());
    } 

    return contentsRect;
}

QSizeF QskRadioBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    auto radio = static_cast<const QskRadioBox*>( skinnable );

    const auto font = skinnable->effectiveFont( Q::Text );
    const auto textMargins = skinnable->marginHint( Q::Text );
    const auto buttonMargins = skinnable->marginHint( Q::Button );
    const auto symbolMargins = skinnable->marginHint( Q::Symbol );
    
    qreal maxTextWidth = 0;
    for(auto& item : radio->items() ) {
	maxTextWidth = std::max( maxTextWidth, qskHorizontalAdvance( font, item ) );
    }

    auto radioWidth = radio->strutSizeHint(Q::Button).width();
    auto symbolWidth = radio->strutSizeHint(Q::Symbol).width();

    maxTextWidth += textMargins.left() + textMargins.right();
    radioWidth += buttonMargins.left() + buttonMargins.right();
    symbolWidth += symbolMargins.left() + symbolMargins.right();

    auto spacing = radio->spacingHint(Q::Panel);
    return QSizeF( maxTextWidth + qMax(radioWidth, symbolWidth),
		   ( lineHeight( radio ) + spacing ) * radio->items().size()
		   - spacing );
}   

QSGNode* QskRadioBoxSkinlet::updateSubNode( const QskSkinnable* skinnable,
						quint8 nodeRole, QSGNode* node) const
{
    auto radio = static_cast<const QskRadioBox*>( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
	    return updateBoxNode( skinnable, node, Q::Panel );

        case ButtonRole:
	    return updateSeriesNode( radio, Q::Button, node );

        case SymbolRole:
	    return updateSeriesNode( radio, Q::Symbol, node );
	    
        case TextRole:
	    return updateSeriesNode( radio, Q::Text, node );

        case RippleRole:
	    return updateBoxNode( radio, node, Q::Ripple );
    };

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

qreal QskRadioBoxSkinlet::lineHeight(const QskRadioBox* target) const {
    auto strutHight = qMax( target->strutSizeHint( Q::Button ).height(),
		 target->strutSizeHint( Q::Text ).height() );
    const auto textMargins = target->marginHint( Q::Text );
    auto fontHeight = target->effectiveFontHeight( Q::Text );
    fontHeight += textMargins.top() + textMargins.bottom();

    return qMax( strutHight, fontHeight );    
}


int QskRadioBoxSkinlet::sampleCount( const QskSkinnable* skinnable,
					 QskAspect::Subcontrol ) const {
    const auto radio = static_cast< const QskRadioBox* >( skinnable );
    return radio->items().count();
}

QRectF QskRadioBoxSkinlet::buttonRect( const QskRadioBox* radio,
				      const QskAspect::Subcontrol target, 
				      const QRectF& rect, int index ) const {
    auto result = rect;
    result.setSize( radio->strutSizeHint( target ) );

    auto spacing = radio->spacingHint(Q::Panel);
    result.moveTop( ( lineHeight( radio ) + spacing ) * index
		    + (lineHeight(radio) - result.size().height()) / 2);

    auto maxWidth = qMax(radio->strutSizeHint( Q::Button ).width(),
			 radio->strutSizeHint( Q::Symbol ).width());
    
    if( radio->layoutMirroring() ) {
	result.moveRight( rect.width() - (maxWidth - result.width())/2);
    } else {
	result.moveLeft((maxWidth - result.width()) / 2);
    }
 
    return result;
}

QRectF QskRadioBoxSkinlet::textRect( const QskRadioBox* radio,
				     const QRectF& rect, int index ) const {
    QRectF result = rect;
    auto spacing = radio->spacingHint(Q::Panel);
    auto lh = lineHeight( radio );
    const auto textMargins = radio->marginHint( Q::Text );

    result.moveTop( index * ( lh + spacing )
		    + lh - radio->effectiveFontHeight(Q::Text)
		    + textMargins.top());

    if( !radio->layoutMirroring() ) {
	auto maxWidth = qMax( buttonRect( radio, Q::Symbol, rect, index ).width(),
			      buttonRect( radio, Q::Button, rect, index ).width());

	result.moveLeft( maxWidth + textMargins.left());
    }

    return result;
}

QRectF QskRadioBoxSkinlet::sampleRect( const QskSkinnable* skinnable,
		   const QRectF& rect, QskAspect::Subcontrol subcontrol,
		   int index ) const {
    const auto radio = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == Q::Text ) {
	return textRect( radio, rect, index );
    } 

    return buttonRect( radio, subcontrol, rect, index);
}

QskAspect::States QskRadioBoxSkinlet::sampleStates( const QskSkinnable* skinnable,
				QskAspect::Subcontrol subControl, int index ) const {
    auto radio = static_cast<const QskRadioBox*>( skinnable );
    auto states =  Inherited::sampleStates( skinnable, subControl, index );

    if( radio->selectedIndex() == index ) {
	return states | Q::Selected;
    }

    return states;
}

QSGNode* QskRadioBoxSkinlet::updateSampleNode(  const QskSkinnable* skinnable,
    QskAspect::Subcontrol subcontrol, int index, QSGNode* node ) const {
    auto radio = static_cast<const QskRadioBox*>( skinnable );

    auto rect = sampleRect( skinnable, radio->contentsRect(),
        subcontrol, index );
    
    if( subcontrol == Q::Text ) {
	return QskSkinlet::updateTextNode( radio,
				node,
				rect,
				Qt::AlignLeft,
				radio->items()[index],
				subcontrol);
    } else if (subcontrol == Q::Button) {
	return QskSkinlet::updateBoxNode(radio,
					 node,
					 rect,
					 subcontrol);
    } else if( subcontrol == Q::Symbol ) {
        auto symbol = QskStandardSymbol::NoSymbol;
	auto color = radio->color( subcontrol ).rgb();

	if( radio->selectedIndex() == index ) {
	    symbol = QskStandardSymbol::Bullet;
	    color = radio->color( subcontrol | Q::Selected ).rgb();
	}
    
	auto graphic = radio->effectiveSkin()->symbol( symbol );

	/*
        Our default skins do not have the concept of colorRoles
        implemented. Until then we do the recoloring manually here
	*/
	QskColorFilter filter;
	filter.addColorSubstitution( Qt::black, color );

	QskGraphic::fromGraphic( graphic, filter );

	return updateGraphicNode( radio, node, graphic, filter, rect );
    }

    return node;
}
