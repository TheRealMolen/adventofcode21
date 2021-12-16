#include "pch.h"
#include "harness.h"

#define D_LOG_OPERATORSx

#ifdef D_LOG_OPERATORS
#define LOGOP(expr)     cout << expr
#else
#define LOGOP(expr) do {} while(0)
#endif

class BitStream
{
    using Bytes = vector<uint8_t>;
    using BytesCIt = Bytes::const_iterator;
    Bytes       m_bytes;
    int         m_nextBit;
    BytesCIt    m_itCurrByte;
    
public:
    BitStream(const string& input)
    {
        for (auto it = input.begin(); it != input.end(); it += 2)
        {
            uint8_t b = 0;
            char c = *it;
            if (isdigit(c))
                b |= uint8_t(c - '0');
            else
                b |= 10 + uint8_t(c - 'A');
            b <<= 4;

            c = *(it + 1);
            if (isdigit(c))
                b |= uint8_t(c - '0');
            else
                b |= 10 + uint8_t(c - 'A');

            m_bytes.push_back(b);
        }

        m_nextBit = 0;
        m_itCurrByte = begin(m_bytes);
    }

    uint32_t readBits(int nBits);

    size_t tell() const
    {
        return (8 * distance(begin(m_bytes), m_itCurrByte)) + m_nextBit;
    }
};

uint32_t BitStream::readBits(int nBits)
{
    if (m_nextBit == 8)
    {
        m_nextBit = 0;
        ++m_itCurrByte;
    }

    uint32_t bits = 0;
    if (m_nextBit + nBits <= 8)
    {
        uint32_t mask = (1 << nBits) - 1;
        bits = ((*m_itCurrByte) >> (8 - (m_nextBit + nBits)));
        bits &= mask;
        m_nextBit += nBits;
    }
    else
    {
        while (m_nextBit + nBits > 8)
        {
            uint32_t chunkBits = 8 - m_nextBit;
            bits <<= chunkBits;
            bits |= readBits(chunkBits);
            nBits -= chunkBits;
        }
        bits <<= nBits;
        bits |= readBits(nBits);
    }
    return bits;
}


int64_t parsePacket(BitStream& stream, uint32_t& totalVersion);

int64_t parseLiteral(BitStream& stream)
{
    int64_t lit = 0;
    bool more = false;
    do {
        uint32_t next = stream.readBits(5);
        more = next >> 4;
        lit <<= 4;
        lit |= next & 15;
    } while (more);

    LOGOP(lit);
    return lit;
}

template<typename Op>
int64_t parseOperator(BitStream& stream, uint32_t& totalVersion, Op opFunc)
{
    bool isLengthInPackets = stream.readBits(1) != 0;
    if (isLengthInPackets)
    {
        int32_t numSubPackets = int32_t(stream.readBits(11));
        bool first = true;
        for ( ; numSubPackets > 0; --numSubPackets)
        {
            if (!first) LOGOP(", ");
            first = false;
            opFunc(parsePacket(stream, totalVersion));
        }
    }
    else
    {
        uint32_t packetLen = uint32_t(stream.readBits(15));
        auto before = stream.tell();
        bool first = true;
        while (stream.tell() < before + packetLen)
        {
            if (!first) LOGOP(", ");
            first = false;
            opFunc(parsePacket(stream, totalVersion));
        }
    }

    return 1;
}


int64_t parsePacket(BitStream& stream, uint32_t& totalVersion)
{
    auto version = stream.readBits(3);
    totalVersion += version;

    auto packetId = stream.readBits(3);
    switch (packetId)
    {
    case 0:
    {
        LOGOP("sum( ");
        int64_t sum = 0;
        parseOperator(stream, totalVersion, [&sum](int64_t val) { sum += val; });
        LOGOP(" )");
        return sum;
    }
    case 1:
    {
        LOGOP("prod( ");
        int64_t prod = 1;
        parseOperator(stream, totalVersion, [&prod](int64_t val) { prod *= val; });
        LOGOP(" )");
        return prod;
    }
    case 2:
    {
        LOGOP("min( ");
        int64_t res = INT64_MAX;
        parseOperator(stream, totalVersion, [&res](int64_t val) { res = min(val, res); });
        LOGOP(" )");
        return res;
    }
    case 3:
    {
        LOGOP("max( ");
        int64_t res = INT64_MIN;
        parseOperator(stream, totalVersion, [&res](int64_t val) { res = max(val, res); });
        LOGOP(" )");
        return res;
    }

    case 4:
        return parseLiteral(stream);

    case 5:
    {
        LOGOP("gt( ");
        int packet = 0;
        int64_t res = INT64_MIN;
        parseOperator(stream, totalVersion, [&res, &packet](int64_t val)
            {
                if (packet == 0)
                    res = val;
                else
                    res = (res > val) ? 1 : 0;
                ++packet;
            });
        LOGOP(" )");
        return res;
    }

    case 6:
    {
        LOGOP("lt( ");
        int packet = 0;
        int64_t res = INT64_MIN;
        parseOperator(stream, totalVersion, [&res, &packet](int64_t val)
            {
                if (packet == 0)
                    res = val;
                else
                    res = (res < val) ? 1 : 0;
                ++packet;
            });
        LOGOP(" )");
        return res;
    }

    case 7:
    {
        LOGOP("eq( ");
        int packet = 0;
        int64_t res = INT64_MIN;
        parseOperator(stream, totalVersion, [&res, &packet](int64_t val)
            {
                if (packet == 0)
                    res = val;
                else
                    res = (res == val) ? 1 : 0;
                ++packet;
            });
        LOGOP(" )");
        return res;
    }

    default:
        return 0xffffffff;
    }
}


int64_t readPacket(const string& input, uint32_t& totalVersion)
{
    BitStream stream(input);

    return parsePacket(stream, totalVersion);

}

int day16(const string& input)
{
    uint32_t totalVersion = 0;
    LOGOP("\n\n");
    readPacket(input, totalVersion);
    LOGOP(endl);
    return int(totalVersion);
}

int64_t day16_2(const string& input)
{
    uint32_t totalVersion = 0;
    LOGOP("\n\n");
    auto res = readPacket(input, totalVersion);
    LOGOP(endl);
    return res;
}


void run_day16()
{
    string sample = "D2FE28";

    test(16, day16("8A004A801A8002F478"));
    test(12, day16("620080001611562C8802118E34"));
    test(23, day16("C0015000016115A2E0802F182340"));
    test(31, day16("A0016C880162017C3686B18A3D4780"));
    gogogo(day16(LOADSTR(16)));

    test(3, day16_2("C200B40A82"));
    test(54, day16_2("04005AC33890"));
    test(7, day16_2("880086C3E88112"));
    test(9, day16_2("CE00C43D881120"));
    test(1, day16_2("D8005AC2A8F0"));
    test(0, day16_2("F600BC2D8F"));
    test(0, day16_2("9C005AC2F8F0"));
    test(1, day16_2("9C0141080250320F1802104A08"));
    gogogo(day16_2(LOADSTR(16)));
}