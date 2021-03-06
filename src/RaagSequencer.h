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
        PARAM_OCTAVE_MAX,
        PARAM_TRANSPOSE,
        NUM_PARAMS
    };
    enum InputIds {
        IN_AROHA_SA,
        IN_AVROHA_SA = IN_AROHA_SA + numArohaInputPorts,
        IN_TRIGGER = IN_AVROHA_SA + numArohaInputPorts,
        IN_DIRECTION,
        IN_RESET,
        NUM_INPUTS
    };
    enum OutputIds {
        OUT_AROHA_SA,
        OUT_AVROHA_SA = OUT_AROHA_SA + numArohaOutputPorts,
        OUT_VOCT = OUT_AVROHA_SA + numArohaOutputPorts,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(LIGHT_AROHA_SA, numArohaOutputPorts * numLightColors),
        ENUMS(LIGHT_AVROHA_SA, numArohaOutputPorts * numLightColors),
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
    float getArohaLightBrightness(Note note, int colorIndex = 0); // default color: Green)
    float getAvrohaLightBrightness(Note note, int colorIndex = 0); // default color: Green)
    void setArohaLightBrightness(Note note, float brightness, int colorIndex = 0); // default color: Green
    void setAvrohaLightBrightness(Note note, float brightness, int colorIndex = 0);

    RaagEngine m_raagEngine;
    dsp::SchmittTrigger m_trigger;
    dsp::SchmittTrigger m_triggerReset;
    std::array<Note, numArohaInputPorts> m_arohaInputLastNotes;
    std::array<Note, numArohaInputPorts> m_avrohaInputLastNotes;
    std::array<int, numArohaOutputPorts> m_arohaNumInputConnections;
    std::array<int, numArohaOutputPorts> m_avrohaNumInputConnections;
    bool m_isFirstStep = true;

};


struct RaagSequencerWidget : ModuleWidget {
    explicit RaagSequencerWidget(RaagSequencer *module);
};

Model* modelRaagSequencer = createModel<RaagSequencer, RaagSequencerWidget>("RaagSequencer");