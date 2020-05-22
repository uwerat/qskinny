#ifndef MAINCONTENT_H
#define MAINCONTENT_H

#include <QskLinearBox.h>

class MainContent : public QskLinearBox
{
    Q_OBJECT

public:
    MainContent( QQuickItem* parent );

private:
    void addCard( const QString& title, QskControl* content, int column = -1 );

    QList< QskLinearBox* > m_columns;
};

#endif // MAINCONTENT_H
