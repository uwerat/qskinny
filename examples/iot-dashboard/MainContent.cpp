#include "MainContent.h"

#include "Card.h"
#include "PieChart.h"
#include "TopBar.h"
#include "Usage.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>

MainContent::MainContent( QQuickItem *parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment(Qt::AlignTop);
    setSpacing(30);

    auto* topBar = new TopBar(this);

    auto* gridBox = new QskGridBox(this);
    gridBox->setMargins({15, 0, 15, 0});
    gridBox->setPanel(true);
    gridBox->setBoxShapeHint(QskBox::Panel, QskBoxShapeMetrics(6) );
    gridBox->setBoxBorderMetricsHint( QskBox::Panel, QskBoxBorderMetrics(2) );
    QskBoxBorderColors borderColors("#dddddd");
    borderColors.setAlpha(100);
    gridBox->setBoxBorderColorsHint( QskBox::Panel, borderColors);

    auto* usage = new Usage(gridBox);
    gridBox->addItem(usage, 0, 0, 2, 1);
}

void MainContent::addCard( const QString &title, QskControl *content, int column )
{
    new Card( title, content, m_columns.at( column ) );
}
