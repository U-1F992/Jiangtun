#include <gtest/gtest.h>

#include "nxmc.h"

constexpr nxmc::RawPacket kValidRawPacket = {0xab, 0, 0, 8, 128, 128, 128, 128, 0, 0, 0};

TEST(PacketTest, ValidRawPacket)
{
    auto raw = kValidRawPacket;
    auto expected = nxmc::Packet::From(raw);
    ASSERT_TRUE(expected.has_value()) << expected.error();

    auto packet = expected.value();
    EXPECT_EQ(packet.GetY(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetB(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetA(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetX(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetL(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetR(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetZL(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetZR(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetMinus(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetPlus(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetLClick(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetRClick(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetHome(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetCapture(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetHat(), nxmc::Hat::Neutral);
    EXPECT_EQ(packet.GetLX(), 128);
    EXPECT_EQ(packet.GetLY(), 128);
    EXPECT_EQ(packet.GetRX(), 128);
    EXPECT_EQ(packet.GetRY(), 128);
    EXPECT_EQ(packet.GetExt0(), 0);
    EXPECT_EQ(packet.GetExt1(), 0);
    EXPECT_EQ(packet.GetExt2(), 0);
}

TEST(PacketTest, PressY)
{
    auto raw = kValidRawPacket;
    raw.at(1) = 1;
    auto expected = nxmc::Packet::From(raw);
    ASSERT_TRUE(expected.has_value());

    auto packet = expected.value();
    EXPECT_EQ(packet.GetY(), nxmc::Button::Pressed);
    EXPECT_EQ(packet.GetB(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetA(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetX(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetL(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetR(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetZL(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetZR(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetMinus(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetPlus(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetLClick(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetRClick(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetHome(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetCapture(), nxmc::Button::Released);
    EXPECT_EQ(packet.GetHat(), nxmc::Hat::Neutral);
    EXPECT_EQ(packet.GetLX(), 128);
    EXPECT_EQ(packet.GetLY(), 128);
    EXPECT_EQ(packet.GetRX(), 128);
    EXPECT_EQ(packet.GetRY(), 128);
    EXPECT_EQ(packet.GetExt0(), 0);
    EXPECT_EQ(packet.GetExt1(), 0);
    EXPECT_EQ(packet.GetExt2(), 0);
}

TEST(PacketTest, InalidHeader)
{
    auto raw = kValidRawPacket;
    raw.at(0) = 0; // must be 0xab
    auto e = nxmc::Packet::From(raw);
    EXPECT_FALSE(e.has_value()) << e.value().ToJSONString();
}

TEST(PacketTest, InvalidButtons)
{
    auto raw = kValidRawPacket;
    raw.at(2) = 0x3F; // 0b00111111 ... OK
    auto e = nxmc::Packet::From(raw);
    EXPECT_TRUE(e.has_value()) << e.error();

    raw.at(2) = 0x40; // 0b01000000 ... NG
    e = nxmc::Packet::From(raw);
    EXPECT_FALSE(e.has_value()) << e.value().ToJSONString();
}

TEST(PacketTest, InvalidHat)
{
    auto raw = kValidRawPacket;
    raw.at(3) = 8; // OK
    auto e = nxmc::Packet::From(raw);
    EXPECT_TRUE(e.has_value()) << e.error();

    raw.at(3) = 9; // must within 8, NG
    e = nxmc::Packet::From(raw);
    EXPECT_FALSE(e.has_value()) << e.value().ToJSONString();
}

TEST(DefineLoopTest, ValidPacketPassing)
{
    nxmc::DefineLoop(
        [](nxmc::LogCallback _)
        {
            auto raw = kValidRawPacket;
            raw.at(1) = 1; // press Y
            return nxmc::Packet::From(raw);
        },
        [](nxmc::NullablePacket nullable_packet, nxmc::LogCallback _)
        {
            EXPECT_TRUE(nullable_packet.has_value());
            if (!nullable_packet.has_value())
            {
                return nxmc::ExpectVoid(); // to suppress compile error
            }

            auto packet = nullable_packet.value();
            EXPECT_EQ(packet.GetY(), nxmc::Button::Pressed);
            EXPECT_EQ(packet.GetB(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetA(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetX(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetL(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetR(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetZL(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetZR(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetMinus(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetPlus(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetLClick(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetRClick(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetHome(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetCapture(), nxmc::Button::Released);
            EXPECT_EQ(packet.GetHat(), nxmc::Hat::Neutral);
            EXPECT_EQ(packet.GetLX(), 128);
            EXPECT_EQ(packet.GetLY(), 128);
            EXPECT_EQ(packet.GetRX(), 128);
            EXPECT_EQ(packet.GetRY(), 128);
            EXPECT_EQ(packet.GetExt0(), 0);
            EXPECT_EQ(packet.GetExt1(), 0);
            EXPECT_EQ(packet.GetExt2(), 0);

            return nxmc::ExpectVoid();
        },
        [](nxmc::Severity _0, const std::string &_1) {})();
}

TEST(DefineLoopTest, InvalidPacketPassing)
{
    nxmc::DefineLoop(
        [](nxmc::LogCallback _)
        {
            auto raw = kValidRawPacket;
            raw.at(0) = 0; // invalid header
            return nxmc::Packet::From(raw);
        },
        [](nxmc::NullablePacket nullable_packet, nxmc::LogCallback _)
        {
            EXPECT_FALSE(nullable_packet.has_value()) << nullable_packet.value().ToJSONString();
            return nxmc::ExpectVoid();
        },
        [](nxmc::Severity _0, const std::string &_1) {})();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if (RUN_ALL_TESTS())
        ;
    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
