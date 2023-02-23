#ifndef JIANGTUN_NXMC_H_
#define JIANGTUN_NXMC_H_

#include "../logger/logger.hpp"

#define LENGTH_ (11)
#define HEADER_ (0xAB)

typedef union
{
    uint8_t raw[LENGTH_];
    struct
    {
        uint8_t header; // must be HEADER_

        uint8_t y : 1;
        uint8_t b : 1;
        uint8_t a : 1;
        uint8_t x : 1;
        uint8_t l : 1;
        uint8_t r : 1;
        uint8_t zl : 1;
        uint8_t zr : 1;

        uint8_t minus : 1;
        uint8_t plus : 1;
        uint8_t lclick : 1;
        uint8_t rclick : 1;
        uint8_t home : 1;
        uint8_t capture : 1;
        uint8_t _ : 2; // unused

        uint8_t hat; // must be between Hat::Up and Hat::Neutral

        uint8_t lx;
        uint8_t ly;

        uint8_t rx;
        uint8_t ry;

        uint8_t ext0;
        uint8_t ext1;
        uint8_t ext2;
    };
} Packet;

enum class Hat : uint8_t
{
    Up,
    UpRight,
    Right,
    DownRight,
    Down,
    DownLeft,
    Left,
    UpLeft,
    Neutral
};

inline bool IsValid(const Packet packet)
{
    return (packet.header == HEADER_) && (packet._ == 0) && (packet.hat <= static_cast<uint8_t>(Hat::Neutral));
}

class PacketHandler
{
public:
    PacketHandler(
        bool (*Receive)(Packet &packet, Logger &logger),
        void (*Filter)(Packet &packet, Logger &logger),
        void (*Send)(const Packet packet, Logger &logger),
        Logger &logger) : Receive_(Receive), Filter_(Filter), Send_(Send), logger_(logger) {}
    void Loop()
    {
        Packet packet;

        if (Receive_(packet, logger_))
        {
            Filter_(packet, logger_);
        }
        Send_(packet, logger_);
    }

private:
    bool (*Receive_)(Packet &packet, Logger &logger);
    void (*Filter_)(Packet &packet, Logger &logger);
    void (*Send_)(const Packet packet, Logger &logger);
    Logger &logger_;
};

#endif // JIANGTUN_NXMC_H_
