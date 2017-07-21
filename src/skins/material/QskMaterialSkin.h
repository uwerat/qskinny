/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include <QskSkin.h>
#include <memory>

class QSK_EXPORT QskMaterialSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

public:
    QskMaterialSkin( QObject* parent = nullptr );
    virtual ~QskMaterialSkin();

private:
    virtual void resetColors( const QColor& accent ) override;

    void initHints();

    void initCommonHints();
    void initTextLabelHints();
    void initFocusIndicatorHints();
    void initSeparatorHints();
    void initPageIndicatorHints();
    void initPushButtonHints();
    void initPopupHints();
    void initDialogButtonHints();
    void initDialogButtonBoxHints();
    void initSliderHints();
    void initTabButtonHints();
    void initTabBarHints();
    void initTabViewHints();
    void initInputPanelHints();
    void initLineEditHints();
    void initScrollViewHints();
    void initListViewHints();
    void initSubWindowHints();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
