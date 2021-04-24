//
// Created by Kaushal Sali on 24/04/21.
//

#include "RaagSequencerExpander.h"


RaagSequencerExpander::RaagSequencerExpander()
{
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    for (int i=0; i<numInputPorts; i++) {
        m_inputLastNotes[i] = Note::NONE;
    }

    leftExpander.producerMessage = m_leftMessages[0];
    leftExpander.consumerMessage = m_leftMessages[1];
}

void RaagSequencerExpander::process(const ProcessArgs &args) {
    if (leftExpander.module && leftExpander.module->model == modelRaagSequencer) {
        // Get consumer message
        auto *message = (float*) leftExpander.producerMessage;
//            for (int i = 0; i < 8; i++) {
//                lights[i].setBrightness(message[i]);
//            }
    }
    else {
        // No mother module is connected.
        // TODO Clear the lights.
    }
}



RaagSequencerExpanderWidget::RaagSequencerExpanderWidget(RaagSequencerExpander *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RaagSequencerExpander.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    for (int i = 0; i < Note::TOTAL; i++) {
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8, panelHeight - (10.f + i * 10))), module,RaagSequencerExpander::IN_SA + i));
    }

}
