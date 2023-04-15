#include <gtest/gtest.h>

#include <functional>
#include <vector>

#include "etl/expected.h"
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Init.h"
#include "plog/Log.h"
#include "plog/Formatters/TxtFormatter.h"

#include "nxmc.h"

std::function<etl::expected<uint8_t, std::string>()> TestInput(const std::vector<etl::expected<uint8_t, std::string>> input)
{
    size_t pos = 0;
    return [input, pos]() mutable -> etl::expected<uint8_t, std::string>
    {
        auto ret = input.at(pos++);
        if (pos == input.size())
        {
            pos = 0;
        }
        return ret;
    };
}

etl::expected<nxmc::Packet, std::string> GetPacket(std::vector<etl::expected<uint8_t, std::string>> input)
{
    auto TryReceive = nxmc::PacketReceiver(TestInput(input));
    etl::expected<nxmc::Packet, std::string> expected;
    for (int i = 0; i < input.size(); i++)
    {
        expected = TryReceive();
    }
    return expected;
}

TEST(PacketReceiverTest, ValidBytes)
{
    auto expected = GetPacket({ 0xAB, 0b11111111, 0b00111111, 8, 128, 128, 128, 128, 255, 255, 255 });
    ASSERT_TRUE(expected.has_value()) << expected.error();

    auto packet = expected.value();
    EXPECT_EQ(packet.header, nxmc::kHeader);
    EXPECT_EQ(packet.y, nxmc::Button::Pressed);
    EXPECT_EQ(packet.b, nxmc::Button::Pressed);
    EXPECT_EQ(packet.a, nxmc::Button::Pressed);
    EXPECT_EQ(packet.x, nxmc::Button::Pressed);
    EXPECT_EQ(packet.l, nxmc::Button::Pressed);
    EXPECT_EQ(packet.r, nxmc::Button::Pressed);
    EXPECT_EQ(packet.zl, nxmc::Button::Pressed);
    EXPECT_EQ(packet.zr, nxmc::Button::Pressed);
    EXPECT_EQ(packet.minus, nxmc::Button::Pressed);
    EXPECT_EQ(packet.plus, nxmc::Button::Pressed);
    EXPECT_EQ(packet.lclick, nxmc::Button::Pressed);
    EXPECT_EQ(packet.rclick, nxmc::Button::Pressed);
    EXPECT_EQ(packet.home, nxmc::Button::Pressed);
    EXPECT_EQ(packet.capture, nxmc::Button::Pressed);
    EXPECT_EQ(packet.hat, nxmc::Hat::Neutral);
    EXPECT_EQ(packet.lx, 128);
    EXPECT_EQ(packet.ly, 128);
    EXPECT_EQ(packet.rx, 128);
    EXPECT_EQ(packet.ry, 128);
    EXPECT_EQ(packet.ext0, 255);
    EXPECT_EQ(packet.ext1, 255);
    EXPECT_EQ(packet.ext2, 255);
}

TEST(PacketReceiverTest, PassThrough)
{
    const std::string kTest = "test";
    auto expected = GetPacket({ etl::expected<uint8_t, std::string>(etl::unexpected<std::string>(kTest)) });
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), kTest);
}

TEST(PacketReceiverTest, HeaderInvalid)
{
    auto e_0 = GetPacket({ 0xAB, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    ASSERT_TRUE(e_0.has_value()) << e_0.error();

    auto e_1 = GetPacket({ 0xFF }); 
    ASSERT_FALSE(e_1.has_value());
}

TEST(PacketReceiverTest, ButtonsOutOfRange)
{
    auto e_0 = GetPacket({ 0xAB, 0, 0b00111111, 0, 0, 0, 0, 0, 0, 0, 0 });
    ASSERT_TRUE(e_0.has_value()) << e_0.error();

    auto e_1 = GetPacket({ 0xAB, 0, 0b01000000 });
    ASSERT_FALSE(e_1.has_value());

    auto e_2 = GetPacket({ 0xAB, 0, 0b11000000 });
    ASSERT_FALSE(e_2.has_value());
}

static plog::ColorConsoleAppender<plog::TxtFormatter> appender(plog::OutputStream::streamStdErr);
    
int main(int argc, char **argv)
{
    plog::init(plog::verbose, &appender);

    ::testing::InitGoogleTest(&argc, argv);
    if (RUN_ALL_TESTS())
        ;
    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
