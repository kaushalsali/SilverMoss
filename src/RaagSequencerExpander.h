//
// Created by Kaushal Sali on 24/04/21.
//

#include "plugin.hpp"

struct RaagSequencerExpander : Module {
public:
    constexpr static int numInputPorts = 24;

    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(IN_SA, numInputPorts),
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS,
    };

    float m_leftMessages[2][8] = {};

    RaagSequencerExpander();

    void process(const ProcessArgs &args) override;
};


struct RaagSequencerExpanderWidget :  ModuleWidget {
    explicit RaagSequencerExpanderWidget(RaagSequencerExpander *module);
    constexpr static float panelWidth = 203.2;    //TODO: Does Rack api provide these from svg?
    constexpr static float panelHeight = 128.5;
};

Model* modelRaagSequencerExpander = createModel<RaagSequencerExpander, RaagSequencerExpanderWidget>("RaagSequencerExpander");

