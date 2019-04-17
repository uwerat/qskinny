/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SQUIEK_SKIN_H
#define QSK_SQUIEK_SKIN_H

#include "QskSquiekGlobal.h"
#include <QskSkin.h>
#include <memory>

class QSK_SQUIEK_EXPORT QskSquiekSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskSquiekSkin( QObject* parent = nullptr );
    ~QskSquiekSkin() override;

  private:
    void resetColors( const QColor& accent ) override;

    void initHints();

    void initCommonHints();

    void initDialogButtonHints();
    void initDialogButtonBoxHints();
    void initFocusIndicatorHints();
    void initInputPanelHints();
    void initInputPredictionBar();
    void initVirtualKeyboardHints();
    void initListViewHints();
    void initPageIndicatorHints();
    void initPopupHints();
    void initPushButtonHints();
    void initScrollViewHints();
    void initSeparatorHints();
    void initSliderHints();
    void initSubWindowHints();
    void initTabButtonHints();
    void initTabBarHints();
    void initTabViewHints();
    void initTextLabelHints();
    void initTextInputHints();

    enum PanelStyle
    {
        NoPanel,
        Raised,
        Sunken,
        Plain,
        Flat
    };

    void setSeparator( QskAspect::Aspect );
    void setButton( QskAspect::Aspect, PanelStyle, qreal border = 2.0 );
    void setPanel( QskAspect::Aspect, PanelStyle );
    void setTab( QskAspect::Aspect );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
