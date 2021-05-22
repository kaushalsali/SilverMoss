//
// Created by Kaushal Sali on 16/01/21.
//

#include "plugin.hpp"
#include "RaagEngine.h"
#include <array>


struct RaagSequencer : Module {
public:
    constexpr static int numArohaInputPorts = 24;
    constexpr static int numArohaOutputPorts = 12;
    constexpr static int numLightColors = 2;    // GreenRed

    enum ParamIds {
        PARAM_OCTAVE_MIN,
        PARAM_OCTAVE_RANGE,
        PARAM_TRANSPOSE,
        PARAM_TRIGGER,
        PARAM_DIRECTION,
        PARAM_RESET,
        PARAM_NUM_STEPS,
        PARAM_BACKTRACKING,
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(IN_AROHA_SA, numArohaInputPorts),
        ENUMS(IN_AVROHA_SA, numArohaInputPorts),
        IN_TRIGGER,
        IN_DIRECTION,
        IN_RESET,
        IN_NUM_STEPS,
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(OUT_AROHA_SA, numArohaOutputPorts),
        ENUMS(OUT_AVROHA_SA, numArohaOutputPorts),
        OUT_VOCT,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(LIGHT_AROHA_SA, numArohaOutputPorts * numLightColors),
        ENUMS(LIGHT_AVROHA_SA, numArohaOutputPorts * numLightColors),
        LIGHT_TRIGGER,
        LIGHT_RESET,
        NUM_LIGHTS,
    };

    static float freqToVolt(float frequencyInHz, bool isLFO=false) {
        return std::log2(frequencyInHz/ (isLFO ? 2.0f : dsp::FREQ_A4));
    }

    static float voltToFreq(float frequencyInHz, bool isLFO=false) {
        return std::log2(frequencyInHz/ (isLFO ? 2.0f : dsp::FREQ_A4));
    }

    static double midiToFreq(int midiNoteNumber) {
        return std::pow(2, static_cast<double>(midiNoteNumber)/12) * 440;
    }

    RaagSequencer();
    void process(const ProcessArgs& args) override;

private:
    void updateConnections();
    float getArohaLightBrightness(Note note, int colorIndex = 0); // default color: Green
    float getAvrohaLightBrightness(Note note, int colorIndex = 0);  //TODO: Use Enum instead of color id
    void setArohaLightBrightness(Note note, float brightness, int colorIndex = 0);
    void setAvrohaLightBrightness(Note note, float brightness, int colorIndex = 0);

    RaagEngine m_raagEngine;
    dsp::SchmittTrigger m_trigTriggerInput;
    dsp::SchmittTrigger m_trigTriggerButton;
    dsp::SchmittTrigger m_trigResetInput;
    dsp::SchmittTrigger m_trigResetButton;
    std::array<Note, numArohaInputPorts> m_arohaInputLastNotes;
    std::array<Note, numArohaInputPorts> m_avrohaInputLastNotes;
    std::array<int, numArohaOutputPorts> m_arohaNumInputConnections;
    std::array<int, numArohaOutputPorts> m_avrohaNumInputConnections;
    bool m_isFirstStep = true;
    float m_resetLightBrightness = 0.f;
    float m_triggerLightBrightness = 0.f;

};


struct RaagSequencerWidget : ModuleWidget {
    explicit RaagSequencerWidget(RaagSequencer *module);
    constexpr static float panelWidth = 203.2;    //TODO: Does Rack api provide these from svg?
    constexpr static float panelHeight = 128.5;
};

Model* modelRaagSequencer = createModel<RaagSequencer, RaagSequencerWidget>("RaagSequencer");