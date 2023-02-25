#include "QskRadioBoxSkinlet.h"

#include "QskAspect.h"
#include "QskRadioBox.h"

#include "QskSkinStateChanger.h"
#include "QskStandardSymbol.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskFunctions.h"
#include "QskSkin.h"

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

QskAspect::States statesForIndex( const QskRadioBox* radio, int index ) {
    auto states = radio->skinStates();
    
    if( radio->selectedIndex() == index ) {
	states |= Q::Selected;
    }

    if( radio->pressedIndex() == index ) {
	states |= Q::Pressed;
    }

    if( radio->positionHint( Q::Ripple ) == index ) {
	states |= Q::Focused;
    }

    return states;
}

QRectF QskRadioBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol ) const
{
    auto radio = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == Q::Ripple ) {
	return rippleRect( radio, contentsRect );	
    } 

    return contentsRect;
}

QSizeF QskRadioBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    auto radio = static_cast< const QskRadioBox* >( skinnable );

    const auto font = skinnable->effectiveFont( Q::Text );
    const auto textMargins = skinnable->marginHint( Q::Text );
    const auto buttonMargins = skinnable->marginHint( Q::Button );
    const auto symbolMargins = skinnable->marginHint( Q::Symbol );
			 
    qreal maxTextWidth = 0;
    for( auto& item : radio->items() ) {
	maxTextWidth = std::max( maxTextWidth, qskHorizontalAdvance( font, item ) );
    }

    auto buttonWidth = radio->strutSizeHint( Q::Button ).width();
    auto symbolWidth = radio->strutSizeHint( Q::Symbol ).width();

    maxTextWidth += textMargins.left() + textMargins.right();
    buttonWidth += buttonMargins.left() + buttonMargins.right();
    symbolWidth += symbolMargins.left() + symbolMargins.right();

    auto spacing = radio->spacingHint( Q::Panel );
    return QSizeF( maxTextWidth + qMax( buttonWidth, symbolWidth ),
        ( lineHeight( radio ) + spacing ) * radio->items().size() - spacing );
}   

QSGNode* QskRadioBoxSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    auto radio = static_cast< const QskRadioBox* >( skinnable );

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
	{
	    QskSkinStateChanger changer( radio );
	    auto ripplePosition = radio->positionHint( Q::Ripple );
	    changer.setStates( statesForIndex( radio, ripplePosition ) );

	    return updateBoxNode( radio, node, Q::Ripple );
	}
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

int QskRadioBoxSkinlet::sampleCount( const QskSkinnable* skinnable, QskAspect::Subcontrol ) const {
    const auto radio = static_cast< const QskRadioBox* >( skinnable );
    return radio->items().count();
}

QSizeF QskRadioBoxSkinlet::buttonSymbolSize( const QskRadioBox* radio ) const {
    auto buttonStrut = radio->strutSizeHint( Q::Button );
    auto symbolStrut = radio->strutSizeHint( Q::Symbol );

    buttonStrut = buttonStrut.grownBy( radio->marginHint( Q::Button ) );
    symbolStrut = symbolStrut.grownBy( radio->marginHint( Q::Symbol ) );

    return QSizeF(
        qMax( buttonStrut.width(), symbolStrut.width() ),
        qMax( buttonStrut.height(), symbolStrut.height() ) );
}

QRectF QskRadioBoxSkinlet::rippleRect( const QskRadioBox* radio, const QRectF& rect ) const {
    auto ripplePosition = radio->positionHint( Q::Ripple );

    if( ripplePosition < 0 ) {
	return QRectF();
    }
    
    auto button = buttonRect( radio, Q::Button, rect, ripplePosition );
    auto rippleSize = radio->strutSizeHint( Q::Ripple );
    button.moveLeft( button.x() - ( rippleSize.width() - button.width() ) / 2 );
    button.moveTop( button.y() - ( rippleSize.height() - button.height() ) / 2 );
    button.setSize( rippleSize );
    return button;
}

QRectF QskRadioBoxSkinlet::buttonRect( const QskRadioBox* radio,
    const QskAspect::Subcontrol target, const QRectF& rect, double index ) const {
    if( index < 0 ) {
	return QRectF();
    }

    auto result = rect;
    result.setSize( radio->strutSizeHint( target ) );

    auto spacing = radio->spacingHint( Q::Panel );
    result.moveTop( ( lineHeight( radio ) + spacing ) * index );

    auto margins = radio->marginHint( target );
    auto withMargins = result.size().grownBy( margins );

    auto maxSize = buttonSymbolSize( radio );
    auto alignment = radio->alignmentHint( target );

    // Vertical positioning
    auto alignHeight = maxSize.height() - withMargins.height();
    if( alignment.testFlag( Qt::AlignVCenter ) ) {
	result.moveTop( result.top() + alignHeight / 2 );
    } else if( alignment.testFlag( Qt::AlignBottom ) ) {
	result.moveTop( result.top() + alignHeight );
    }
    result.moveTop( result.top() + margins.top() );

    // Horizontal positioning
    auto alignWidth = 0;
    if( alignment.testFlag( Qt::AlignHCenter ) ) {
	alignWidth = ( maxSize.width() - withMargins.width() ) / 2;
    } else if ( alignment.testFlag( Qt::AlignRight )) {
	alignWidth = maxSize.width() - withMargins.width();
    }

    if( radio->layoutMirroring() ) {
	result.moveRight( rect.width() - ( alignWidth + margins.right() ) );
    } else {
	result.moveLeft( margins.left() + alignWidth );	
    }
 
    return result;
}

QRectF QskRadioBoxSkinlet::textRect( const QskRadioBox* radio,
    const QRectF& rect, int index ) const {
    auto text = radio->items()[ index ];

    if(text.isEmpty()) {
	return QRectF();
    }

    QRectF result = rect;
    auto spacing = radio->spacingHint( Q::Panel );
    auto lh = lineHeight( radio );
    const auto textMargins = radio->marginHint( Q::Text );
    const auto font = radio->effectiveFont( Q::Text );

    result.moveTop( index * ( lh + spacing )
        + lh - radio->effectiveFontHeight( Q::Text )
	+ textMargins.top());

    result.setHeight( lh );
    result.setWidth( qskHorizontalAdvance( font, text ) );

    auto button = buttonRect( radio, Q::Button, rect, index );
    auto buttonsMargins = radio->marginHint( Q::Button );
    auto buttonWidth = button.marginsAdded( buttonsMargins ).width();

    if( radio->layoutMirroring() ) {
	result.moveLeft( rect.width() - textMargins.right()
	    - result.width() - buttonWidth);
    } else {
	result.moveLeft( buttonWidth + textMargins.left() );
    }

    return result;
}

QRectF QskRadioBoxSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subcontrol, int index ) const {

    const auto radio = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == Q::Text ) {
	return textRect( radio, rect, index );
    } 

    return buttonRect( radio, subcontrol, rect, index);
}

QskAspect::States QskRadioBoxSkinlet::sampleStates( const QskSkinnable* skinnable,
				QskAspect::Subcontrol subControl, int index ) const {

    auto radio = static_cast< const QskRadioBox* >( skinnable );
    auto states =  Inherited::sampleStates( skinnable, subControl, index );

    return states | statesForIndex( radio, index );
}

QSGNode* QskRadioBoxSkinlet::updateSampleNode(  const QskSkinnable* skinnable,
    QskAspect::Subcontrol subcontrol, int index, QSGNode* node ) const {

    auto radio = static_cast< const QskRadioBox* >( skinnable );

    auto rect = sampleRect( skinnable, radio->contentsRect(),
        subcontrol, index );
    
    if( subcontrol == Q::Text ) {
	return QskSkinlet::updateTextNode( radio, node, rect, Qt::AlignLeft,
	    radio->items()[ index ], subcontrol );
    } else if ( subcontrol == Q::Button ) {
	return QskSkinlet::updateBoxNode( radio, node, rect, subcontrol );
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
	auto colorSub = radio->color( subcontrol | statesForIndex( radio, index ) );
	filter.addColorSubstitution( Qt::black, colorSub.rgb() );

	QskGraphic::fromGraphic( graphic, filter );

	return updateGraphicNode( radio, node, graphic, filter, rect );
    }

    return node;
}

#include "moc_QskRadioBoxSkinlet.cpp"
