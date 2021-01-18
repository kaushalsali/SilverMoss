//
// Created by Kaushal Sali on 16/01/21.
//

#include "plugin.hpp"
#include "RaagEngine.h"
#include <array>


struct RaagSequencer : Module {
private:
    constexpr static int numArohaInputPorts = 24;
    constexpr static int numArohaOutputPorts = 12;

public:
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
        NUM_INPUTS
    };
    enum OutputIds {
        OUT_AROHA_SA,
        OUT_AVROHA_SA = OUT_AROHA_SA + numArohaOutputPorts,
        OUT_VOCT = OUT_AVROHA_SA + numArohaOutputPorts,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
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

    RaagEngine m_raagEngine;
    dsp::SchmittTrigger m_trigger;
    std::array<float, numArohaInputPorts> m_arohaInputLastValues;
    std::array<float, numArohaInputPorts> m_avrohaInputLastValues;
    int m_lastTransposeValue = 0;
    bool m_isFirstStep = true;

};


struct RaagSequencerWidget : ModuleWidget {
    explicit RaagSequencerWidget(RaagSequencer *module);
};

Model* modelRaagSequencer = createModel<RaagSequencer, RaagSequencerWidget>("RaagSequencer");