#include <gtest/gtest.h>

#include "nxmc/helper.h"

constexpr nxmc::RawPacket kValidRawPacket = {0xab, 0, 0, 8, 128, 128, 128, 128, 0, 0, 0};

TEST(DefineLogTest, NeverWriteWhenLowestIsNone)
{
    auto log = nxmc::helper::DefineLog(
        [](const std::string &_0)
        {
            FAIL();
        },
        nxmc::Severity::None);
    log(nxmc::Severity::Debug, "");
    log(nxmc::Severity::Info, "");
    log(nxmc::Severity::Warning, "");
    log(nxmc::Severity::Error, "");
    log(nxmc::Severity::None, "");
}

TEST(DefineLogTest, NeverWriteDebugOrInfoWhenLowestIsWarning)
{
    auto log = nxmc::helper::DefineLog(
        [](const std::string &message)
        {
            if (
                message.find("DEBUG") != std::string::npos ||
                message.find("INFO") != std::string::npos)
            {
                FAIL();
            }
        },
        nxmc::Severity::Warning);
    log(nxmc::Severity::Debug, "");
    log(nxmc::Severity::Info, "");
    log(nxmc::Severity::Warning, "");
    log(nxmc::Severity::Error, "");
    log(nxmc::Severity::None, "");
}

TEST(DefineRetrieveTest, ValidRawPacket)
{
    auto read = []()
    {
        auto raw = kValidRawPacket;
        raw.at(10) = 0xFF; // identify
        return raw;
    };
    auto log = [](nxmc::Severity _0, const std::string &_1) {};

    auto must_be_packet = nxmc::helper::DefineRetrieve(read)(log);
    ASSERT_TRUE(must_be_packet.has_value()) << must_be_packet.error();

    auto packet = must_be_packet.value();
    EXPECT_EQ(packet.GetExt2(), 0xFF) << packet.ToJSONString();
}

TEST(DefineRetrieveTest, InvalidRawPacket)
{
    std::string kReason = "test";
    auto read = [kReason]()
    {
        return nxmc::ExpectRawPacket(nxmc::UnexpectedReason(kReason));
    };
    auto log = [](nxmc::Severity _0, const std::string &_1) {};

    auto e = nxmc::helper::DefineRetrieve(read)(log);
    ASSERT_FALSE(e.has_value());
    EXPECT_EQ(e.error(), kReason) << e.error();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if (RUN_ALL_TESTS())
        ;
    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
