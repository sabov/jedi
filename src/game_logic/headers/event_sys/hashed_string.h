#ifndef HASHED_STRING_H
#define HASHED_STRING_H

#include <cctype>
#include <cstring>
#include <string>

namespace GameLogic {

class CHashedString ;
//this one here just defines an event-wildcard
const std::string kpWildCardEventType("event_wild_card");

class CHashedString {
private:
    void*       m_ident         ;   //hash value
    char const* m_identString   ;

public:
    explicit CHashedString ( char const * const pIdentString ) : m_ident ( hash_name( pIdentString ) ),
        m_identString(pIdentString)
    {}

    unsigned long getHashValue() const
    {
        return reinterpret_cast<unsigned long>(m_identString);
    }

    char const * const getString() const {return m_identString;}

    bool operator< ( CHashedString const & o ) const
    {
        return ( getHashValue() < o.getHashValue() );
    }

    bool operator== ( CHashedString const & o ) const
    {
        return ( getHashValue() == o.getHashValue() );
    }

    static void * hash_name( char const * pIdentString )
    {
        //largest prime smaller than 65536
        unsigned long BASE = 65521L;

        //NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32 -1
        unsigned long NMAX= 5552;

    #define DO1(buf,i) {s1 += tolower(buf[i]); s2 += s1;}
    #define DO2(buf,i) DO1(buf,i); DO1(buf,i+1);
    #define DO4(buf,i) DO2(buf,i); DO2(buf,i+2);
    #define DO8(buf,i) DO4(buf,i); DO4(buf,i+4);
    #define DO16(buf)  DO8(buf,0); DO8(buf,8);

        if ( pIdentString == 0 )
            return 0 ;

        if ( strcmp( pIdentString, kpWildCardEventType.c_str() ) == 0 )
            return 0 ;

        unsigned long s1 = 0;
        unsigned long s2 = 0;

        for ( size_t len = strlen( pIdentString ); len > 0 ; )
        {
            unsigned long k = len < NMAX ? len : NMAX;
            len -= k ;
            while ( k >= 16 )
            {
                DO16(pIdentString);
                pIdentString += 16;
                k -= 16;
            }

            if ( k != 0 ) do
            {
                s1 += *pIdentString++;
                s2 += s1;
            } while (--k);

            s1 %= BASE;
            s2 %= BASE;
        }
#ifdef _MSC_VER
    #pragma warning(push);
    #pragma warning(disable : 4312)
#endif
        return reinterpret_cast<void*>( (s2 << 16) | s1 );
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
    #undef DO1
    #undef DO2
    #undef DO4
    #undef DO8
    #undef DO16
    }
};


} //Namespace

#endif // HASHED_STRING_H
