#include <gtest/gtest.h>
#include "PluginProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace test_plugins
{

    TEST(Juce, Oscillator)
    {
        juce::dsp::Oscillator<float> osc;
        EXPECT_FALSE(osc.isInitialised());
        osc.initialise([](float x)
                       { return x; });
        EXPECT_TRUE(osc.isInitialised());
        osc.setFrequency(1);
        EXPECT_EQ(1.0, osc.getFrequency());
    }

} // namespace test_plugins