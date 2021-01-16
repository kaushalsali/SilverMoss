//
// Created by Kaushal Sali on 16/01/21.
//

#include "plugin.hpp"


struct RaagSequencer : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    RaagSequencer();
    void process(const ProcessArgs& args) override;
};


struct RaagSequencerWidget : ModuleWidget {
    explicit RaagSequencerWidget(RaagSequencer *module);
};

Model* modelRaagSequencer = createModel<RaagSequencer, RaagSequencerWidget>("RaagSequencer");