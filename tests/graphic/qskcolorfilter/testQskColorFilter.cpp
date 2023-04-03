#include <QskColorFilter.h>

#include <QBrush>
#include <QTest>

class TestQskColorFilter : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void substituteBrush();
};

void TestQskColorFilter::substituteBrush()
{
    QskColorFilter filter;
    filter.addColorSubstitution( Qt::white, Qt::black );

    QLinearGradient gradient;
    QGradientStops originalStops( { { 0.0, Qt::white }, { 1.0, Qt::blue } } );
    gradient.setStops( originalStops );

    QBrush originalBrush( gradient );

    auto newBrush = filter.substituted( originalBrush );
    QGradientStops newStops( { { 0.0, Qt::black }, { 1.0, Qt::blue } } );

    QVERIFY( originalBrush.gradient()->stops() == originalStops );
    QVERIFY( newBrush.gradient()->stops() == newStops );
}

QTEST_GUILESS_MAIN( TestQskColorFilter )
#include "testQskColorFilter.moc"
