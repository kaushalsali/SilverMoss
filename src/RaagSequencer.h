//
// Created by Kaushal Sali on 16/01/21.
//

#include "plugin.hpp"
#include "RaagEngine.h"
#include <array>


struct RaagSequencer : Module {
public:
    enum ParamIds {
        PARAM_OCTAVE_MIN,
        PARAM_OCTAVE_MAX,
        NUM_PARAMS
    };
    enum InputIds {
        IN_AROHA_SA,
        IN_AVROHA_SA = IN_AROHA_SA + 12,
        IN_TRIGGER = IN_AVROHA_SA + 12,
        IN_DIRECTION,
        NUM_INPUTS
    };
    enum OutputIds {
        OUT_AROHA_SA,
        OUT_AVROHA_SA = IN_AROHA_SA + 12,
        OUT_VOCT = IN_AVROHA_SA + 12,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    static float freqToVolt(float frequencyInHz, bool isLFO=false) {
        return std::log2(frequencyInHz/ (isLFO ? 2.0f : dsp::FREQ_C4));
    }

    static float voltToFreq(float frequencyInHz, bool isLFO=false) {
        return std::log2(frequencyInHz/ (isLFO ? 2.0f : dsp::FREQ_C4));
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
    std::array<float, static_cast<int>(Note::TOTAL)> m_arohaInputLastValues;
    std::array<float, static_cast<int>(Note::TOTAL)> m_avrohaInputLastValues;
    bool m_isFirstStep = true;

};


struct RaagSequencerWidget : ModuleWidget {
    explicit RaagSequencerWidget(RaagSequencer *module);
};

Model* modelRaagSequencer = createModel<RaagSequencer, RaagSequencerWidget>("RaagSequencer");