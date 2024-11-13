/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             HandlingMidiEventsTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Handles incoming midi events.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include <iostream>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "JucePluginDefines.h"
#include "JuceHeader.h"

//==============================================================================
class MainProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MainProcessor()
        : AudioProcessor(BusesProperties()) // add no audio buses at all
    {
    }

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        ignoreUnused(samplesPerBlock);
        ignoreUnused(sampleRate);
    }

    void releaseResources() override {}

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi) override
    {
        ignoreUnused(buffer);
        ignoreUnused(midi);
        // A pure MIDI plugin shouldn't be provided any audio data
        jassert(buffer.getNumChannels() == 0);
        midi.clear();

        // however we use the buffer to get timing information
        // auto numSamples = buffer.getNumSamples();

        // get note duration
        // auto noteDuration = static_cast<int>(std::ceil(rate * 0.25f * (0.1f + (1.0f - (*speed)))));

        // for (const auto metadata : midi)
        // {
        //     const auto msg = metadata.getMessage();
        //     if (msg.isNoteOn())
        //         notes.add(msg.getNoteNumber());
        //     else if (msg.isNoteOff())
        //         notes.removeValue(msg.getNoteNumber());
        // }


        // if ((time + numSamples) >= noteDuration)
        // {
        //     auto offset = jmax(0, jmin((int)(noteDuration - time), numSamples - 1));

        //     if (lastNoteValue > 0)
        //     {
        //         midi.addEvent(MidiMessage::noteOff(1, lastNoteValue), offset);
        //         lastNoteValue = -1;
        //     }

        //     if (notes.size() > 0)
        //     {
        //         currentNote = (currentNote + 1) % notes.size();
        //         lastNoteValue = notes[currentNote];
        //         midi.addEvent(MidiMessage::noteOn(1, lastNoteValue, (uint8)127), offset);
        //     }
        // }

        // time = (time + numSamples) % noteDuration;
    }

    using juce::AudioProcessor::processBlock;

    //==============================================================================
    bool isMidiEffect() const override { return true; }

    //==============================================================================
    AudioProcessorEditor *createEditor() override { return new MainContentComponent(*this); }
    bool hasEditor() const override { return true; }

    //==============================================================================
    const String getName() const override { return "MainProcessor"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return "None"; }
    void changeProgramName(int, const String &) override {}

    //==============================================================================
    void getStateInformation(MemoryBlock &destData) override
    {
        ignoreUnused(destData);
    }

    void setStateInformation(const void *data, int sizeInBytes) override
    {
        ignoreUnused(data);
        ignoreUnused(sizeInBytes);
    }

private:
    //==============================================================================

    //==============================================================================

    class MainContentComponent : public juce::AudioProcessorEditor,
                                 private juce::MidiInputCallback,
                                 private juce::MidiKeyboardStateListener
    {
    public:
        MainContentComponent(MainProcessor &p)
            : AudioProcessorEditor(&p),
              keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
              startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
        {
            setOpaque(true);

            addAndMakeVisible(midiInputListLabel);
            midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
            midiInputListLabel.attachToComponent(&midiInputList, true);

            addAndMakeVisible(midiInputList);
            midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
            auto midiInputs = juce::MidiInput::getAvailableDevices();

            juce::StringArray midiInputNames;

            for (auto input : midiInputs)
                midiInputNames.add(input.name);

            midiInputList.addItemList(midiInputNames, 1);
            midiInputList.onChange = [this]
            { setMidiInput(midiInputList.getSelectedItemIndex()); };

            // find the first enabled device and use that by default
            for (auto input : midiInputs)
            {
                if (deviceManager.isMidiInputDeviceEnabled(input.identifier))
                {
                    setMidiInput(midiInputs.indexOf(input));
                    break;
                }
            }

            // if no enabled devices were found just use the first one in the list
            if (midiInputList.getSelectedId() == 0)
                setMidiInput(0);

            addAndMakeVisible(keyboardComponent);
            keyboardState.addListener(this);

            addAndMakeVisible(midiMessagesBox);
            midiMessagesBox.setMultiLine(true);
            midiMessagesBox.setReturnKeyStartsNewLine(true);
            midiMessagesBox.setReadOnly(true);
            midiMessagesBox.setScrollbarsShown(true);
            midiMessagesBox.setCaretVisible(false);
            midiMessagesBox.setPopupMenuEnabled(true);
            midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
            midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
            midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

            setSize(600, 400);
        }

        ~MainContentComponent() override
        {
            keyboardState.removeListener(this);
            deviceManager.removeMidiInputDeviceCallback(juce::MidiInput::getAvailableDevices()[midiInputList.getSelectedItemIndex()].identifier, this);
            if (midiDevice != nullptr)
                midiDevice->sendMessageNow(disableDAW);
        }

        void paint(juce::Graphics &g) override
        {
            g.fillAll(juce::Colours::black);
        }

        void resized() override
        {
            auto area = getLocalBounds();

            midiInputList.setBounds(area.removeFromTop(36).removeFromRight(getWidth() - 150).reduced(8));
            keyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
            midiMessagesBox.setBounds(area.reduced(8));
        }

    private:
        static juce::String getMidiMessageDescription(const juce::MidiMessage &m)
        {
            if (m.isNoteOn())
                return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
            if (m.isNoteOff())
                return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
            if (m.isProgramChange())
                return "Program change " + juce::String(m.getProgramChangeNumber());
            if (m.isPitchWheel())
                return "Pitch wheel " + juce::String(m.getPitchWheelValue());
            if (m.isAftertouch())
                return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
            if (m.isChannelPressure())
                return "Channel pressure " + juce::String(m.getChannelPressureValue());
            if (m.isAllNotesOff())
                return "All notes off";
            if (m.isAllSoundOff())
                return "All sound off";
            if (m.isMetaEvent())
                return "Meta event";

            if (m.isController())
            {
                juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

                if (name.isEmpty())
                    name = "[" + juce::String(m.getControllerNumber()) + "]";

                return "Controller " + name + ": " + juce::String(m.getControllerValue());
            }

            return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
        }

        void logMessage(const juce::String &m)
        {
            midiMessagesBox.moveCaretToEnd();
            midiMessagesBox.insertTextAtCaret(m + juce::newLine);
        }

        /** Starts listening to a MIDI input device, enabling it if necessary. */
        void setMidiInput(int index)
        {
            auto list = juce::MidiInput::getAvailableDevices();

            for (auto newInput : list)
            {
                deviceManager.removeMidiInputDeviceCallback(newInput.identifier, this);
                if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
                    deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
                deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
                midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

                if (newInput.name.equalsIgnoreCase("Launchpad Mini MK3 LPMiniMK3 DA"))
                {

                    midiDevice = juce::MidiOutput::openDevice(newInput.identifier);
                    midiDevice->sendMessageNow(disableDAW);
                    midiDevice->sendMessageNow(enableDAW);
                    // midiDevice->sendMessageNow(fader);
                    // midiDevice->sendMessageNow(enableFader);
                    midiDevice->sendMessageNow(selectSessionLayout);
                    midiDevice->sendMessageNow(clearSession);
                    int channel = 1;
                    for (uint row = 0; row < 8; ++row)
                    {
                        for (uint col = 0; col < 8; ++col)
                        {
                            u_char note = 11 + (row * 8) + col;
                            u_char color = 44 + (col * 8) + row;
                            juce::MidiMessage msg{new uint8[9]{0x90, note, color}, 3};
                            msg.setChannel(channel);
                            midiDevice->sendMessageNow(msg);
                        }
                        channel++;
                        if (channel > 3)
                            channel = 1;
                    }
                }
            }

            lastInputIndex = index;
        }

        // These methods handle callbacks from the midi device + on-screen keyboard..
        void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override
        {
            const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
            keyboardState.processNextMidiEvent(message);
            postMessageToList(message, source->getName());
        }

        void handleNoteOn(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity) override
        {
            if (!isAddingFromMidiInput)
            {
                auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
                m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
                postMessageToList(m, "On-Screen Keyboard");
            }
        }

        void handleNoteOff(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float /*velocity*/) override
        {
            if (!isAddingFromMidiInput)
            {
                auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
                m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
                postMessageToList(m, "On-Screen Keyboard");
            }
        }

        // This is used to dispach an incoming message to the message thread
        class IncomingMessageCallback : public juce::CallbackMessage
        {
        public:
            IncomingMessageCallback(MainContentComponent *o, const juce::MidiMessage &m, const juce::String &s)
                : owner(o), message(m), source(s)
            {
            }

            void messageCallback() override
            {
                if (owner != nullptr)
                    owner->addMessageToList(message, source);
            }

            Component::SafePointer<MainContentComponent> owner;
            juce::MidiMessage message;
            juce::String source;
        };

        void postMessageToList(const juce::MidiMessage &message, const juce::String &source)
        {
            (new IncomingMessageCallback(this, message, source))->post();
        }

        void addMessageToList(const juce::MidiMessage &message, const juce::String &source)
        {
            auto time = message.getTimeStamp() - startTime;

            auto hours = ((int)(time / 3600.0)) % 24;
            auto minutes = ((int)(time / 60.0)) % 60;
            auto seconds = ((int)time) % 60;
            auto millis = ((int)(time * 1000.0)) % 1000;

            auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d",
                                                    hours,
                                                    minutes,
                                                    seconds,
                                                    millis);

            auto description = getMidiMessageDescription(message);

            juce::String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
            logMessage(midiMessageString);
        }

        //==============================================================================
        juce::AudioDeviceManager deviceManager; // [1]
        juce::ComboBox midiInputList;           // [2]
        juce::Label midiInputListLabel;
        int lastInputIndex = 0;             // [3]
        bool isAddingFromMidiInput = false; // [4]

        juce::MidiKeyboardState keyboardState;         // [5]
        juce::MidiKeyboardComponent keyboardComponent; // [6]
        std::unique_ptr<juce::MidiOutput> midiDevice{nullptr};

        juce::TextEditor midiMessagesBox;
        double startTime;
        static const uint8 enableDAWSysex[];
        static const uint8 disableDAWSysex[];
        static const uint8 faderSysex[];
        static const uint8 enableFaderSysex[];
        static const uint8 clearSessionSysex[];
        static const uint8 selectSessionLayoutSysex[];
        static const juce::MidiMessage enableDAW;
        static const juce::MidiMessage disableDAW;
        static const juce::MidiMessage fader;
        static const juce::MidiMessage enableFader;
        static const juce::MidiMessage clearSession;
        static const juce::MidiMessage selectSessionLayout;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessor)
};
const uint8 MainProcessor::MainContentComponent::enableDAWSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x10, 0x01, 0xF7};
const uint8 MainProcessor::MainContentComponent::disableDAWSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x10, 0x00, 0xF7};
const uint8 MainProcessor::MainContentComponent::faderSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x01, 0x00, 0x00,
                                                                 0x00, 0x00, 0x07, 0x25,
                                                                 0x01, 0x00, 0x08, 0x26,
                                                                 0x02, 0x00, 0x09, 0x27,
                                                                 0x03, 0x00, 0x10, 0x28,
                                                                 0x04, 0x01, 0x11, 0x29,
                                                                 0x05, 0x01, 0x12, 0x30,
                                                                 0x06, 0x01, 0x13, 0x31,
                                                                 0x07, 0x01, 0x14, 0x32,
                                                                 0xF7};
const uint8 MainProcessor::MainContentComponent::enableFaderSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x00, 0x0D, 0xF7};
const uint8 MainProcessor::MainContentComponent::clearSessionSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x12, 0x01, 0x00, 0x00, 0xF7};
const uint8 MainProcessor::MainContentComponent::selectSessionLayoutSysex[] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D, 0x00, 0x00, 0xF7};
const juce::MidiMessage MainProcessor::MainContentComponent::enableDAW{MainContentComponent::enableDAWSysex, 9};
const juce::MidiMessage MainProcessor::MainContentComponent::disableDAW{MainContentComponent::disableDAWSysex, 9};
const juce::MidiMessage MainProcessor::MainContentComponent::fader{MainContentComponent::faderSysex, 10 + 4 * 8};
const juce::MidiMessage MainProcessor::MainContentComponent::enableFader{MainContentComponent::enableFaderSysex, 9};
const juce::MidiMessage MainProcessor::MainContentComponent::clearSession{MainContentComponent::clearSessionSysex, 11};
const juce::MidiMessage MainProcessor::MainContentComponent::selectSessionLayout{MainContentComponent::selectSessionLayoutSysex, 9};
