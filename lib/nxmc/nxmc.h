#ifndef NXMC_NXMC_H_
#define NXMC_NXMC_H_

#include <array>
#include <etl/expected.h>
#include <etl/optional.h>

namespace nxmc
{
    typedef etl::unexpected<std::string> UnexpectedReason;

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

    enum class Button : uint8_t
    {
        Released,
        Pressed
    };

    constexpr size_t kRawLength = 11;
    typedef std::array<uint8_t, kRawLength> RawPacket;
    typedef etl::expected<RawPacket, std::string> ExpectRawPacket;

    class Packet
    {
    public:
        typedef etl::expected<Packet, std::string> ExpectPacket;

        /// @brief The factory method of nxmc::Packet.
        /// @param raw
        /// @return nxmc::Packet if raw is valid data.
        static inline ExpectPacket From(const RawPacket raw)
        {
            // Using factory methods and std::expected<T, E> instead of ctor,
            // we can eliminate exceptions throwing which should be avoided in embedded development.

            auto header = raw.at(0);
            auto padding = (raw.at(2) >> 6) & 0b11;
            auto hat = raw.at(3);

            if (header != 0xAB)
            {
                return FailedToParse_("invalid header: ", header);
            }
            else if (padding != 0)
            {
                return FailedToParse_("padding is not 0: ", padding);
            }
            else if (static_cast<uint8_t>(Hat::Neutral) < hat)
            {
                return FailedToParse_("hat is out of range: ", hat);
            }
            else
            {
                return Packet(raw);
            }
        }
        inline Button GetY() const { return y_; }
        inline Button GetB() const { return b_; }
        inline Button GetA() const { return a_; }
        inline Button GetX() const { return x_; }
        inline Button GetL() const { return l_; }
        inline Button GetR() const { return r_; }
        inline Button GetZL() const { return zl_; }
        inline Button GetZR() const { return zr_; }
        inline Button GetMinus() const { return minus_; }
        inline Button GetPlus() const { return plus_; }
        inline Button GetLClick() const { return lclick_; }
        inline Button GetRClick() const { return rclick_; }
        inline Button GetHome() const { return home_; }
        inline Button GetCapture() const { return capture_; }
        inline Hat GetHat() const { return hat_; }
        inline uint8_t GetLX() const { return lx_; }
        inline uint8_t GetLY() const { return ly_; }
        inline uint8_t GetRX() const { return rx_; }
        inline uint8_t GetRY() const { return ry_; }
        inline uint8_t GetExt0() const { return ext0_; }
        inline uint8_t GetExt1() const { return ext1_; }
        inline uint8_t GetExt2() const { return ext2_; }
        std::string ToJSONString()
        {
            char buffer[234]; // each %d will be within 3 chars, 0~255
            snprintf(
                buffer, sizeof(buffer) / sizeof(char),
                "{\"header\":%d,\"y\":%d,\"b\":%d,\"a\":%d,\"x\":%d,\"l\":%d,\"r\":%d,\"zl\":%d,\"zr\":%d,\"plus\":%d,\"minus\":%d,\"lclick\":%d,\"rclick\":%d,\"home\":%d,\"capture\":%d,\"hat\":%d,\"lx\":%d,\"ly\":%d,\"rx\":%d,\"ry\":%d,\"ext0\":%d,\"ext1\":%d,\"ext2\":%d}",
                header_, static_cast<uint8_t>(y_), static_cast<uint8_t>(b_), static_cast<uint8_t>(a_), static_cast<uint8_t>(x_), static_cast<uint8_t>(l_), static_cast<uint8_t>(r_), static_cast<uint8_t>(zl_), static_cast<uint8_t>(zr_), static_cast<uint8_t>(plus_), static_cast<uint8_t>(minus_), static_cast<uint8_t>(lclick_), static_cast<uint8_t>(rclick_), static_cast<uint8_t>(home_), static_cast<uint8_t>(capture_), static_cast<uint8_t>(hat_), lx_, ly_, rx_, ry_, ext0_, ext1_, ext2_);
            return std::string(buffer);
        }

    private:
        static inline ExpectPacket FailedToParse_(const std::string &reason, uint8_t val)
        {
            auto message = reason + std::to_string(val);
            return ExpectPacket(UnexpectedReason(message));
        }
        Packet(const RawPacket raw)
            : header_(raw.at(0)),
              y_(static_cast<Button>(raw.at(1) & 1)),
              b_(static_cast<Button>((raw.at(1) >> 1) & 1)),
              a_(static_cast<Button>((raw.at(1) >> 2) & 1)),
              x_(static_cast<Button>((raw.at(1) >> 3) & 1)),
              l_(static_cast<Button>((raw.at(1) >> 4) & 1)),
              r_(static_cast<Button>((raw.at(1) >> 5) & 1)),
              zl_(static_cast<Button>((raw.at(1) >> 6) & 1)),
              zr_(static_cast<Button>((raw.at(1) >> 7) & 1)),

              minus_(static_cast<Button>(raw.at(2) & 1)),
              plus_(static_cast<Button>((raw.at(2) >> 1) & 1)),
              lclick_(static_cast<Button>((raw.at(2) >> 2) & 1)),
              rclick_(static_cast<Button>((raw.at(2) >> 3) & 1)),
              home_(static_cast<Button>((raw.at(2) >> 4) & 1)),
              capture_(static_cast<Button>((raw.at(2) >> 5) & 1)),

              hat_(static_cast<Hat>(raw.at(3))),

              lx_(raw.at(4)),
              ly_(raw.at(5)),
              rx_(raw.at(6)),
              ry_(raw.at(7)),

              ext0_(raw.at(8)),
              ext1_(raw.at(9)),
              ext2_(raw.at(10))
        {
        }
        uint8_t header_;
        Button y_;
        Button b_;
        Button a_;
        Button x_;
        Button l_;
        Button r_;
        Button zl_;
        Button zr_;
        Button minus_;
        Button plus_;
        Button lclick_;
        Button rclick_;
        Button home_;
        Button capture_;
        Hat hat_;
        uint8_t lx_;
        uint8_t ly_;
        uint8_t rx_;
        uint8_t ry_;
        uint8_t ext0_;
        uint8_t ext1_;
        uint8_t ext2_;
    };

    typedef Packet::ExpectPacket ExpectPacket;
    typedef etl::optional<Packet> NullablePacket;

    enum class Severity : uint8_t
    {
        Debug,
        Info,
        Warning,
        Error,
        None,
    };

    typedef std::function<void(Severity, const std::string &)> LogCallback;
    typedef std::function<ExpectPacket(const LogCallback &)> RetrieveCallback;
    typedef etl::expected<void, std::string> ExpectVoid;
    typedef std::function<ExpectVoid(NullablePacket, const LogCallback &)> SendCallback;

    /// @brief Define loop().
    /// @param retrieve Callback function to retrieve nxmc::Packet. It normally returns nxmc::Packet, or std::string in case of failure.
    /// @param send Callback function to send nxmc::Packet. Note that it will be called each time (with nullopt in that case) even if retrieve fails.
    /// @param log Callback function to write log. It just takes std::string and returns void.
    /// @return
    inline std::function<void(void)> DefineLoop(
        const RetrieveCallback &retrieve,
        const SendCallback &send,
        const LogCallback &log)
    {
        return [retrieve, send, log]()
        {
            auto packet_retrieved = retrieve(log);
            NullablePacket packet;

            if (packet_retrieved.has_value())
            {
                log(Severity::Debug, packet_retrieved.value().ToJSONString());
                packet = packet_retrieved.value();
            }
            else
            {
                log(Severity::Warning, packet_retrieved.error());
                packet = etl::nullopt;
            }

            auto result = send(packet, log);
            if (!result.has_value())
            {
                log(Severity::Warning, result.error());
            }
        };
    }
}

#endif // NXMC_NXMC_H_
