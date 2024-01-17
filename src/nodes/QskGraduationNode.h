/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRADUATION_NODE_H
#define QSK_GRADUATION_NODE_H

#include "QskGlobal.h"
#include "QskBasicLinesNode.h"

#include <qnamespace.h>

class QRectF;
class QskIntervalF;
class QskTickmarks;
class QskGraduationMetrics;

class QSK_EXPORT QskGraduationNode : public QskBasicLinesNode
{
    using Inherited = QskBasicLinesNode;

  public:
    enum Alignment
    {
        Leading,
        Centered,
        Trailing
    };

    QskGraduationNode();
    ~QskGraduationNode() override;

#if 1
    // finding better names
    void setAxis( Qt::Orientation, qreal pos, const QTransform& );
    void setTickMetrics( Alignment, const QskGraduationMetrics& );
#endif

    void update( const QskTickmarks&, const QskIntervalF& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
