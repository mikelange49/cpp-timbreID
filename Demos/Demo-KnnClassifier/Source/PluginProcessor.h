/*
  ==============================================================================

  Plugin Processor

  DEMO PROJECT - TimbreID - bark Module

  Author: Domenico Stefani (domenico.stefani96 AT gmail.com)
  Date: 25th March 2020

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "juce_timbreID.h"

//#define USE_AUBIO_ONSET //If this commented, the bark onset detector is used, otherwise the aubio onset module is used

//==============================================================================
/**
*/
class DemoProcessor : public AudioProcessor,
#ifdef USE_AUBIO_ONSET
                      public tid::aubio::Onset<float>::Listener
#else
                      public tid::Bark<float>::Listener
#endif
{
public:
    //=========================== Juce System Stuff ============================
    DemoProcessor();
    ~DemoProcessor();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    const unsigned int WINDOW_SIZE = 1024;
    const float BARK_SPACING = 0.5;
    //=================== Onset module initialization ==========================

   #ifdef USE_AUBIO_ONSET
    /**    Set initial parameters      **/
    const unsigned int HOP = 128;
    const float ONSET_THRESHOLD = 0.0f;
    const float ONSET_MINIOI = 0.020f;  //20 ms debounce
    const float SILENCE_THRESHOLD = -80.0f;
    const tid::aubio::OnsetMethod ONSET_METHOD = tid::aubio::OnsetMethod::defaultMethod;

    tid::aubio::Onset<float> aubioOnset{WINDOW_SIZE,HOP,ONSET_THRESHOLD,ONSET_MINIOI,SILENCE_THRESHOLD,ONSET_METHOD};

    void onsetDetected (tid::aubio::Onset<float> *);
   #else
    /**    Set initial parameters      **/
    const unsigned int HOP = 128;

    /**    Initialize the onset detector      **/
    tid::Bark<float> bark{WINDOW_SIZE, HOP, BARK_SPACING};
    void onsetDetected (tid::Bark<float>* bark, unsigned int samplesFromPeak);
   #endif
    void onsetDetectedRoutine();

    /**    Initialize the modules      **/
    tid::Bfcc<float> bfcc{WINDOW_SIZE, BARK_SPACING};
    tid::KNNclassifier knn;

    /**    This atomic is used to update the onset LED in the GUI **/
    std::atomic<bool> onsetMonitorState{false};

    std::atomic<unsigned int> matchAtomic{0};
    std::atomic<float> distAtomic{-1.0f};

    /**    Only the first n features of bfcc are used  **/
    int featuresUsed = 50;

    enum class CState {
        idle,
        train,
        classify
    };

    CState classifierState = CState::idle;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoProcessor)
};
