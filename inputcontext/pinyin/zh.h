#ifndef PINYIN_H
#define PINYIN_H

#include <QChar>
#include <QString>

enum Tone: std::uint8_t
{
    NoTone = 0,
    FlatTone = 0x1,
    RisingTone = 0x2,
    LowTone = 0x4,
    FallingTone = 0x8,
    NeutralTone = 0x10
};

struct PinyinCandidateSection
{
    const std::uint16_t* data;
    const char size;
};
using PinyinCandidateSections = PinyinCandidateSection[6];

struct PinyinCandidates
{
    const PinyinCandidateSection* sections;
    const std::int8_t vowel;
    const std::uint8_t subtones;
};

PinyinCandidates pinyinCandidates( const QByteArray& text );

Qt::Key vowelWithTone( QChar vowel, int toneIndex );

#endif // PINYIN_H
