#ifndef MAINCONTENT_H
#define MAINCONTENT_H

#include <QskLinearBox.h>

class MainContent : public QskLinearBox
{
        Q_OBJECT

    public:
        MainContent( QQuickItem* parent );

    private:
        QList< QskLinearBox* > m_columns;
};

#endif // MAINCONTENT_H
