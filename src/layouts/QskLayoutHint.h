#ifndef QSK_LAYOUT_HINT_H
#define QSK_LAYOUT_HINT_H

#include "QskGlobal.h"
#include <qglobal.h>

class QSK_EXPORT QskLayoutHint
{
  public:
    QskLayoutHint();
    QskLayoutHint( qreal minimum, qreal preferred, qreal maximum );

    void intersect( const QskLayoutHint& );
    void normalize();

    qreal size( int which ) const;
    void setSize( int which, qreal size );

    bool operator==( const QskLayoutHint& ) const;
    bool operator!=( const QskLayoutHint& ) const;

    void setMinimum( qreal value );
    qreal minimum() const;

    void setPreferred( qreal value );
    qreal preferred() const;

    void setMaximum( qreal value );
    qreal maximum() const;

  private:
    qreal m_minimum;
    qreal m_preferred;
    qreal m_maximum;
};

inline qreal QskLayoutHint::minimum() const
{   
    return m_minimum;
}           
            
inline void QskLayoutHint::setMinimum( qreal value )
{       
    m_minimum = value;
}           
            
inline qreal QskLayoutHint::preferred() const
{           
    return m_preferred;
}           
        
inline void QskLayoutHint::setPreferred( qreal value )
{   
    m_preferred = value;
}

inline qreal QskLayoutHint::maximum() const
{   
    return m_maximum;
}

inline void QskLayoutHint::setMaximum( qreal value )
{   
    m_maximum = value;
}

inline bool QskLayoutHint::operator==( const QskLayoutHint& other ) const
{
    return ( m_preferred == other.m_preferred )
        && ( m_minimum == other.m_minimum )
        && ( m_maximum == other.m_maximum );
}

inline bool QskLayoutHint::operator!=( const QskLayoutHint& other ) const
{
    return !( *this == other );
}

Q_DECLARE_TYPEINFO( QskLayoutHint, Q_MOVABLE_TYPE );

#endif
