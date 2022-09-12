/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SUBCONTROL_LAYOUT_ENGINE_H
#define QSK_SUBCONTROL_LAYOUT_ENGINE_H

#include "QskGlobal.h"
#include "QskLayoutEngine2D.h"
#include "QskLayoutElement.h"
#include "QskAspect.h"

#include <qnamespace.h>
#include <memory>

class QskSkinnable;

/*
    For the moment this layout is tailored for arranging one text and one graphic
    horizontally/vertically. Candidate for becoming something more general in the future..
 */
class QskSubcontrolLayoutEngine : public QskLayoutEngine2D
{
    using Inherited = QskLayoutEngine2D;

  public:
    class LayoutElement : public QskLayoutElement
    {
      public:
        LayoutElement( const QskSkinnable*, const QskAspect::Subcontrol );

        inline const QskSkinnable* skinnable() const { return m_skinnable; }
        inline QskAspect::Subcontrol subControl() const { return m_subControl; }

        inline void setSizePolicy(
            QskSizePolicy::Policy horizontalPolicy,
            QskSizePolicy::Policy verticalPolicy )
        {
            setSizePolicy( QskSizePolicy( horizontalPolicy, verticalPolicy ) );
        }

        inline void setSizePolicy( QskSizePolicy policy ) { m_sizePolicy = policy; }
        inline QskSizePolicy sizePolicy() const override { return m_sizePolicy; }

        virtual Qt::Alignment alignment() const override;

        inline void setStretch( int stretch ) { m_stretch = stretch; }
        inline int stretch() const { return m_stretch; }

        inline void setGeometry( const QRectF& rect ) { m_geometry = rect; }
        inline const QRectF& geometry() const { return m_geometry; }

        void setMaximumSize( const QSizeF& );
        void setMinimumSize( const QSizeF& );
        void setPreferredSize( const QSizeF& );
        void setFixedSize( const QSizeF& );

        void setExplicitSizeHint( Qt::SizeHint, const QSizeF& );

      private:
        QSizeF sizeHint( Qt::SizeHint, const QSizeF& ) const override;
        virtual QSizeF implicitSize( const QSizeF& ) const = 0;

        int m_stretch = -1;

        QskSizePolicy m_sizePolicy;

        QSizeF m_explicitSizeHints[3];
        QRectF m_geometry;

        const QskSkinnable* m_skinnable;
        const QskAspect::Subcontrol m_subControl;
    };

    class TextElement : public LayoutElement
    {
      public:
        TextElement( const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
            : LayoutElement( skinnable, subControl )
        {
        }

        inline void setText( const QString& text ) { m_text = text; }
        inline QString text() const { return m_text; }

      private:
        QSizeF implicitSize( const QSizeF& ) const override;
        QString m_text;
    };

    class GraphicElement : public LayoutElement
    {
      public:
        GraphicElement( const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
            : LayoutElement( skinnable, subControl )
        {
        }

        inline void setSourceSize( const QSizeF& size ) { m_sourceSize = size; }
        inline QSizeF sourceSize() const { return m_sourceSize; }

        QSizeF effectiveStrutSize() const;

      private:
        QSizeF implicitSize( const QSizeF& ) const override;

        QSizeF m_sourceSize;
    };

    explicit QskSubcontrolLayoutEngine( Qt::Orientation );
    ~QskSubcontrolLayoutEngine() override;

    Qt::Orientation orientation() const;
    bool setOrientation( Qt::Orientation );

    void setSpacing( qreal );
    qreal spacing() const;

    void addElement( LayoutElement* );
    LayoutElement* elementAt( int ) const;
    LayoutElement* element( QskAspect::Subcontrol ) const;

    int count() const override final;

    void setGraphicTextElements( const QskSkinnable*,
        QskAspect::Subcontrol, const QString& text,
        QskAspect::Subcontrol, const QSizeF& graphicSize );

    QRectF subControlRect( QskAspect::Subcontrol ) const;

  private:
    QskSizePolicy sizePolicyAt( int index ) const override;
    void layoutItems() override;

    int effectiveCount( Qt::Orientation ) const override;

    void invalidateElementCache() override;

    virtual void setupChain( Qt::Orientation,
        const QskLayoutChain::Segments&, QskLayoutChain& ) const override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
