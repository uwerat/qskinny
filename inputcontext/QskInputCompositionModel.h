#ifndef QSK_INPUT_COMPOSITION_MODEL_H
#define QSK_INPUT_COMPOSITION_MODEL_H

#include <QObject>

#include <memory>

class QStringList;

class QskInputCompositionModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVector< Qt::Key > groups READ groups NOTIFY groupsChanged )

public:
    QskInputCompositionModel();
    virtual ~QskInputCompositionModel();

    void commit( const QString& );
    void commitCandidate( int );
    void composeKey( Qt::Key );

    void clearPreedit();

    virtual int candidateCount() const;
    virtual Qt::Key candidate( int ) const;

    int groupIndex() const;
    void setGroupIndex(int groupIndex);
    virtual bool nextGroupIndex( int&, bool = true ) const;

    virtual QVector< Qt::Key > groups() const;

protected:
    // Used for text composition
    virtual bool hasIntermediate() const;
    virtual QString polishPreedit( const QString& preedit );
    virtual bool isComposable( const QStringRef& preedit ) const;

Q_SIGNALS:
    void groupsChanged( const QVector< Qt::Key >& );
    void candidatesChanged();

private:
    void backspace();
    void moveCursor( Qt::Key key );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif // QSK_INPUT_COMPOSITION_MODEL_H
