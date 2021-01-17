
#include "RaagSequencer.h"

RaagSequencer::RaagSequencer() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
}

void RaagSequencer::process(const Module::ProcessArgs &args) {
    Module::process(args);
}



RaagSequencerWidget::RaagSequencerWidget(RaagSequencer* module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RaagSequencer.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    for (int i=0; i<12; i++) {
        // Aroha
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 10 + i * 10)), module, RaagSequencer::IN_AROHA_SA + i));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.24, 10 + i * 10)), module, RaagSequencer::OUT_AROHA_SA + i));
        // Avroha
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150 - 35.24, 9.5 + i * 10)), module, RaagSequencer::OUT_AVROHA_SA + i));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(150 - 15.24, 10 + i * 10)), module, RaagSequencer::IN_AVROHA_SA + i));
    }

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(150.f/2, 10 + 2 * 10)), module, RaagSequencer::IN_TRIGGER));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(150/2, 10 + 4 * 10)), module, RaagSequencer::IN_GATE));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(150/2, 10 + 6 * 10)), module, RaagSequencer::IN_DIRECTION));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150/2, 10 + 8 * 10)), module, RaagSequencer::OUT_VOCT));

    //addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 46.063)), module, RaagSequencer::PITCH_PARAM));
    //addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, RaagSequencer::BLINK_LIGHT));
}
