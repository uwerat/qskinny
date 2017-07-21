#include "zh.h"

template< int n >
constexpr PinyinCandidateSection section( const std::uint16_t( &arr )[n] )
{
    static_assert( n <= 127, "Check the character map or increase the permitted size of the PinyinCandidateList." );
    return { arr, n };
}

struct CandidateGroup
{
    const PinyinCandidateSections a;
    const PinyinCandidateSections ai;
    const PinyinCandidateSections an;
    const PinyinCandidateSections ang;
    const PinyinCandidateSections ao;
    const PinyinCandidateSections e;
    const PinyinCandidateSections ei;
    const PinyinCandidateSections en;
    const PinyinCandidateSections eng;
    const PinyinCandidateSections er;
    const PinyinCandidateSections i;
    const PinyinCandidateSections ia;
    const PinyinCandidateSections ian;
    const PinyinCandidateSections iang;
    const PinyinCandidateSections iao;
    const PinyinCandidateSections ie;
    const PinyinCandidateSections in;
    const PinyinCandidateSections ing;
    const PinyinCandidateSections io;
    const PinyinCandidateSections iong;
    const PinyinCandidateSections iu;
    const PinyinCandidateSections o;
    const PinyinCandidateSections ong;
    const PinyinCandidateSections ou;
    const PinyinCandidateSections u;
    const PinyinCandidateSections ua;
    const PinyinCandidateSections uai;
    const PinyinCandidateSections uan;
    const PinyinCandidateSections uang;
    const PinyinCandidateSections ue;
    const PinyinCandidateSections ui;
    const PinyinCandidateSections un;
    const PinyinCandidateSections uo;
    const PinyinCandidateSections v;
    const PinyinCandidateSections ve;
};

enum PinyinInitial
{
    Initial_invalid = -1,
    Initial_none = 0,
    Initial_hMask = 0x80,
    Initial_b = 'b',
    Initial_c = 'c',
    Initial_ch = 'c' + Initial_hMask,
    Initial_d = 'd',
    Initial_f = 'f',
    Initial_g = 'g',
    Initial_h = 'h',
    Initial_j = 'j',
    Initial_k = 'k',
    Initial_l = 'l',
    Initial_m = 'm',
    Initial_n = 'n',
    Initial_p = 'p',
    Initial_q = 'q',
    Initial_r = 'r',
    Initial_s = 's',
    Initial_sh = 's' + Initial_hMask,
    Initial_t = 't',
    Initial_w = 'w',
    Initial_x = 'x',
    Initial_y = 'y',
    Initial_z = 'z',
    Initial_zh = 'z' + Initial_hMask
};

// Helpers to create an enum from a preedit string
inline constexpr std::int32_t makePinyinFinal( const char(&c)[2] )
{ return c[0]; }
inline constexpr std::int32_t makePinyinFinal( const char(&c)[3] )
{ return c[1] << 8 | c[0]; }
inline constexpr std::int32_t makePinyinFinal( const char(&c)[4] )
{ return c[2] << 16 | c[1] << 8 | c[0]; }
inline constexpr std::int32_t makePinyinFinal( const char(&c)[5] )
{ return c[3] << 24 | c[2] << 16 | c[1] << 8 | c[0]; }

enum PinyinFinal : std::int32_t
{
    Final_invalid = -1,
    Final_empty = 0,
    Final_a = makePinyinFinal("a"),
    Final_ai = makePinyinFinal("ai"),
    Final_an = makePinyinFinal("an"),
    Final_ang = makePinyinFinal("ang"),
    Final_ao = makePinyinFinal("ao"),
    Final_e = makePinyinFinal("e"),
    Final_ei = makePinyinFinal("ei"),
    Final_en = makePinyinFinal("en"),
    Final_eng = makePinyinFinal("eng"),
    Final_er = makePinyinFinal("er"),
    Final_ia = makePinyinFinal("ia"),
    Final_ian = makePinyinFinal("ian"),
    Final_iang = makePinyinFinal("iang"),
    Final_iao = makePinyinFinal("iao"),
    Final_i = makePinyinFinal("i"),
    Final_ie = makePinyinFinal("ie"),
    Final_in = makePinyinFinal("in"),
    Final_ing = makePinyinFinal("ing"),
    Final_io = makePinyinFinal("io"),
    Final_ion = makePinyinFinal("ion"), // intermediate to iong
    Final_iong = makePinyinFinal("iong"),
    Final_iu = makePinyinFinal("iu"),
    Final_o = makePinyinFinal("o"),
    Final_on = makePinyinFinal("on"), // intermediate to ong
    Final_ong = makePinyinFinal("ong"),
    Final_ou = makePinyinFinal("ou"),
    Final_ua = makePinyinFinal("ua"),
    Final_uai = makePinyinFinal("uai"),
    Final_uan = makePinyinFinal("uan"),
    Final_uang = makePinyinFinal("uang"),
    Final_u = makePinyinFinal("u"),
    Final_ue = makePinyinFinal("ue"),
    Final_ui = makePinyinFinal("ui"),
    Final_un = makePinyinFinal("un"),
    Final_uo = makePinyinFinal("uo"),
    Final_v = makePinyinFinal("v"),
    Final_ve = makePinyinFinal("ve"),
};

// character codes are split into separate files based on the Initial segment
#include "zh_none.cpp"
#include "zh_b.cpp"
#include "zh_c.cpp"
#include "zh_ch.cpp"
#include "zh_d.cpp"
#include "zh_f.cpp"
#include "zh_g.cpp"
#include "zh_h.cpp"
#include "zh_j.cpp"
#include "zh_k.cpp"
#include "zh_l.cpp"
#include "zh_m.cpp"
#include "zh_n.cpp"
#include "zh_p.cpp"
#include "zh_q.cpp"
#include "zh_r.cpp"
#include "zh_s.cpp"
#include "zh_sh.cpp"
#include "zh_t.cpp"
#include "zh_w.cpp"
#include "zh_x.cpp"
#include "zh_y.cpp"
#include "zh_z.cpp"
#include "zh_zh.cpp"

using namespace zh;

static PinyinInitial pinyinInitial( const QByteArray& text )
{
    const int length = text.length();
    if ( length <= 0 )
        return Initial_invalid;

    switch ( text.at( 0 ) )
    {
        case 'a':
        case 'e':
        case 'o':
        case 'u':
            return Initial_none;
        case 'b':
            return Initial_b;
        case 'c':
            if ( length > 1 && text.at(1) == 'h' )
                return Initial_ch;
            return Initial_c;
        case 'd':
            return Initial_d;
        case 'f':
            return Initial_f;
        case 'g':
            return Initial_g;
        case 'h':
            return Initial_h;
        case 'j':
            return Initial_j;
        case 'k':
            return Initial_k;
        case 'l':
            return Initial_l;
        case 'm':
            return Initial_m;
        case 'n':
            return Initial_n;
        case 'p':
            return Initial_p;
        case 'q':
            return Initial_q;
        case 'r':
            return Initial_r;
        case 's':
            if ( length > 1 && text.at(1) == 'h' )
                return Initial_sh;
            return Initial_s;
        case 't':
            return Initial_t;
        case 'w':
            return Initial_w;
        case 'x':
            return Initial_x;
        case 'y':
            return Initial_y;
        case 'z':
            if ( length > 1 && text.at(1) == 'h' )
                return Initial_zh;
            return Initial_z;
        default:
            break;
    }
    return Initial_invalid;
}

static PinyinFinal pinyinFinal( PinyinInitial i, const QByteArray& text )
{
    // rebuild the string to match the enum
    const int offset = i == Initial_none ? 0 : ( i > Initial_hMask ? 2 : 1 );
    const auto data = text.constData() + offset;
    uint hash = 0;
    switch ( text.length() - offset )
    {
        default: // the ending is too long
            return Final_invalid;
        case 4:
            hash |= *( data + 3 ) << 24;
        // fall through
        case 3:
            hash |= *( data + 2 ) << 16;
        // fall through
        case 2:
            hash |= *( data + 1 ) << 8;
        // fall through
        case 1:
            hash |= *data;
        // fall through
        case 0: // the ending is empty
            break;
    }

    // verify hash
    switch ( static_cast< PinyinFinal >( hash ) )
    {
        case Final_invalid:
            Q_UNREACHABLE();
            break;
        case Final_empty:
        case Final_a:
        case Final_ai:
        case Final_an:
        case Final_ang:
        case Final_ao:
        case Final_e:
        case Final_ei:
        case Final_en:
        case Final_eng:
        case Final_er:
        case Final_ia:
        case Final_ian:
        case Final_iang:
        case Final_iao:
        case Final_i:
        case Final_ie:
        case Final_in:
        case Final_ing:
        case Final_io:
        case Final_ion:
        case Final_iong:
        case Final_iu:
        case Final_o:
        case Final_on:
        case Final_ong:
        case Final_ou:
        case Final_ua:
        case Final_uai:
        case Final_uan:
        case Final_uang:
        case Final_u:
        case Final_ue:
        case Final_ui:
        case Final_un:
        case Final_uo:
        case Final_v:
        case Final_ve:
            return static_cast< PinyinFinal >( hash );
    }

    // all others are invalid
    return Final_invalid;
}

constexpr static std::uint8_t tones( const PinyinCandidateSections& candidates )
{
    return ( candidates[1].data ? FlatTone : NoTone )
           | ( candidates[2].data ? RisingTone : NoTone )
           | ( candidates[3].data ? LowTone : NoTone )
           | ( candidates[4].data ? FallingTone : NoTone )
           | ( candidates[5].data ? NeutralTone : NoTone );
}

constexpr static const PinyinCandidateSections emptySection =
{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

static PinyinCandidates candidatesForFinal(
    const CandidateGroup& group, PinyinFinal final )
{
    // This table is organized such that if there are no candidates for a given
    // ending (final), an empty section is returned (signaling that we are in
    // an intermediate editing phase. If there is only one possible
    // remaining ending (e.g. in->ing), that possible ending is returned.
    // The tone mark is placed by the order: a, o, e, i, u, v (umlaut u).
    // The only exception is "iu" where the tone mark is placed on the u instead.
    static const PinyinCandidates emptyCandidates = { emptySection, 0, 0 };
    switch (final) {
        case Final_empty:
            return emptyCandidates; // Any empty list is returned for intermediate values
        case Final_invalid:
            break;
        case Final_a:
            if ( group.a[0].data )
                return { group.a, 'a', tones( group.a ) };
            return emptyCandidates;
        case Final_ai:
            return { group.ai, 'a', tones(group.ai) };
        case Final_an:
            if ( group.an[0].data )
                return { group.an, 'a', tones(group.an) };
        // fall through
        case Final_ang:
            return { group.ang, 'a', tones(group.ang) };
        case Final_ao:
            return { group.ao, 'a', tones(group.ao) };
        case Final_e:
            if ( group.e[0].data )
                return { group.e, 'e', tones(group.e) };
            return emptyCandidates;
        case Final_ei:
            return { group.ei, 'e', tones(group.ei) };
        case Final_en:
            if ( group.en[0].data )
                return { group.en, 'e', tones(group.en) };
        // fall through
        case Final_eng:
            return { group.eng, 'e', tones(group.eng) };
        case Final_er:
            return { group.er, 'e', tones(group.er) };
        case Final_ia:
            if ( group.ia[0].data )
                return { group.ia, 'a', tones(group.ia) };
            return emptyCandidates;
        case Final_ian:
            if ( group.ian[0].data )
                return { group.ian, 'a', tones(group.ian) };
        // fall through
        case Final_iang:
            return { group.iang, 'a', tones(group.iang) };
        case Final_iao:
            return { group.iao, 'a', tones(group.iao) };
        case Final_i:
            if ( group.i[0].data )
                return { group.i, 'i', tones(group.i) };
            return emptyCandidates;
        case Final_ie:
            return { group.ie, 'e', tones(group.ie) };
        case Final_in:
            if ( group.in[0].data )
                return { group.in, 'i', tones(group.in) };
        // fall through
        case Final_ing:
            return { group.ing, 'i', tones(group.ing) };
        case Final_io:
            if ( group.io[0].data )
                return { group.io, 'o', tones(group.io) };
        // fall through
        case Final_ion:
        case Final_iong:
            return { group.iong, 'o', tones(group.iong) };
        case Final_iu:
            return { group.iu, 'u', tones(group.iu) };
        case Final_o:
            if (group.o[0].data)
                return { group.o, 'o', tones(group.o) };
            return emptyCandidates;
        case Final_on:
        case Final_ong:
            return { group.ong, 'o', tones(group.ong) };
        case Final_ou:
            return { group.ou, 'o', tones(group.ou) };
        case Final_ua:
            if ( group.ua[0].data )
                return { group.ua, 'a', tones(group.ua) };
            return emptyCandidates;
        case Final_uai:
            return { group.uai, 'a', tones(group.uai) };
        case Final_uan:
            if ( group.uan[0].data )
                return { group.uan, 'a', tones(group.uan) };
        // fall through
        case Final_uang:
            return { group.uang, 'a', tones(group.uang) };
        case Final_u:
            if ( group.u[0].data )
                return { group.u, 'u', tones(group.u) };
            return emptyCandidates;
        case Final_ue:
            return { group.ue, 'e', tones(group.ue) };
        case Final_ui:
            return { group.ui, 'i', tones(group.ui) };
        case Final_un:
            return { group.un, 'u', tones(group.un) };
        case Final_uo:
            return { group.uo, 'o', tones(group.uo) };
        case Final_v:
            if ( group.v[0].data )
                return { group.v, 'v', tones(group.v) };
        // fall through
        case Final_ve:
            return { group.ve, 'e', tones(group.ve) };
    }
    return { emptySection, -1, 0 };
}

PinyinCandidates pinyinCandidates( const QByteArray& text )
{
    const auto initial = pinyinInitial( text );
    const auto final = pinyinFinal( initial, text );
    switch ( initial )
    {
        case Initial_invalid:
        case Initial_hMask:
            break;
        case Initial_none:
            return candidatesForFinal( zh::none, final );
        case Initial_b:
            return candidatesForFinal( zh::b, final );
        case Initial_c:
            return candidatesForFinal( zh::c, final );
        case Initial_ch:
            return candidatesForFinal (zh::ch, final );
        case Initial_d:
            return candidatesForFinal( zh::d, final );
        case Initial_f:
            return candidatesForFinal( zh::f, final );
        case Initial_g:
            return candidatesForFinal( zh::g, final );
        case Initial_h:
            return candidatesForFinal( zh::h, final );
        case Initial_j:
            return candidatesForFinal( zh::j, final );
        case Initial_k:
            return candidatesForFinal( zh::k, final );
        case Initial_l:
            return candidatesForFinal( zh::l, final);
        case Initial_m:
            return candidatesForFinal( zh::m, final );
        case Initial_n:
            return candidatesForFinal( zh::n, final );
        case Initial_p:
            return candidatesForFinal( zh::p, final );
        case Initial_q:
            return candidatesForFinal( zh::q, final );
        case Initial_r:
            return candidatesForFinal( zh::r, final );
        case Initial_s:
            return candidatesForFinal( zh::s, final );
        case Initial_sh:
            return candidatesForFinal( zh::sh, final );
        case Initial_t:
            return candidatesForFinal( zh::t, final );
        case Initial_w:
            return candidatesForFinal( zh::w, final );
        case Initial_x:
            return candidatesForFinal( zh::x, final );
        case Initial_y:
            return candidatesForFinal( zh::y, final );
        case Initial_z:
            return candidatesForFinal( zh::z, final );
        case Initial_zh:
            return candidatesForFinal( zh::zh, final );
    }
    return { emptySection, -1, 0 };
}

static constexpr const std::uint16_t vowelsWithTones[][5] =
{
    { 0x0061, 0x0101, 0x00E1, 0x01CE, 0x00E0 }, // a
    { 0x0065, 0x0113, 0x00E9, 0x011B, 0x00E8 }, // e
    { 0x0069, 0x012B, 0x00ED, 0x01D0, 0x00EC }, // i
    { 0x006F, 0x014D, 0x00F3, 0x01D2, 0x00F2 }, // o
    { 0x0075, 0x016B, 0x00FA, 0x01D4, 0x00F9 }, // u
    { 0x00FC, 0x01D6, 0x01D8, 0x01DA, 0x01DC }  // umlaut u
};

Qt::Key vowelWithTone( QChar vowel, int toneIndex )
{
    if ( toneIndex == 5 )
        toneIndex = 0;

    if ( toneIndex >= 0 && toneIndex <= 4 )
    {
        switch ( vowel.toLatin1() )
        {
            case 'a':
                return Qt::Key( vowelsWithTones[ 0 ][ toneIndex ] );
            case 'e':
                return Qt::Key( vowelsWithTones[ 1 ][ toneIndex ] );
            case 'i':
                return Qt::Key( vowelsWithTones[ 2 ][ toneIndex ] );
            case 'o':
                return Qt::Key( vowelsWithTones[ 3 ][ toneIndex ] );
            case 'u':
                return Qt::Key( vowelsWithTones[ 4 ][ toneIndex ] );
            case 'v':
                return Qt::Key( vowelsWithTones[ 5 ][ toneIndex ] );
        }
    }

    return Qt::Key( vowel.toLatin1() );
}
