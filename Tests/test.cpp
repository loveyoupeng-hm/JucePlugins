#include <gtest/gtest.h>
#include "PluginProcessor.h"

namespace test_plugins
{

    TEST(Simple, Foo)
    {
        AudioPluginAudioProcessor processor{};
        EXPECT_TRUE(true);
    }
    
} // namespace test_plugins