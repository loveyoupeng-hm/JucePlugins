#include "ArpeggiatorPluginDemo.h"


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Arpeggiator();
}