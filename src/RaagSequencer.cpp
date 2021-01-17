
#include "RaagSequencer.h"

RaagSequencer::RaagSequencer() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    for (int i=0; i<Note::TOTAL; i++) {
        m_arohaInputLastValues[i] = -1.f;   // -1 indicates that note has been disconnected from the graph
        m_avrohaInputLastValues[i] = -1.f;
    }
    m_isFirstStep = true;

    DEBUG("\n-----------Aroha\"-----------\n%s", m_raagEngine.getAroha().printGraph().c_str());
    DEBUG("\n-----------Avroha\"-----------\n%s", m_raagEngine.getAvroha().printGraph().c_str());

}

void RaagSequencer::process(const Module::ProcessArgs &args) {

    // Set voltage for semitone outputs
    for (int i=0; i<12; i++) {
        outputs[OUT_AROHA_SA + i].setVoltage(static_cast<float>(i) / 12.0f);
        outputs[OUT_AVROHA_SA + i].setVoltage(static_cast<float>(i) / 12.0f);
    }

    // Update connections
    if (!m_isFirstStep)
        updateConnections();
    else
        m_isFirstStep = false;

    // Step if triggered
    if (inputs[IN_TRIGGER].isConnected() && m_trigger.process(inputs[IN_TRIGGER].getVoltage())) {//
        auto directionUp = true;
        if (inputs[IN_DIRECTION].isConnected()) {
            directionUp = inputs[IN_DIRECTION].getVoltage() >= 5.0f;

        }

        //DEBUG("direction = %f", inputs[IN_DIRECTION].getVoltage());
        DEBUG("\n-----------Aroha\"-----------\n%s", m_raagEngine.getAroha().printGraph().c_str());
        DEBUG("\n-----------Avroha\"-----------\n%s", m_raagEngine.getAvroha().printGraph().c_str());

        DEBUG("\nDirection: %i\n", static_cast<int>(directionUp));
        DEBUG("\nNote Played: %s\n", note_map.at(m_raagEngine.getCurrentNote()).c_str());

        int numTries = 3;       //TODO: Improve logic
        bool success = false;
        while(!success && numTries) {
            if (directionUp)
                success = m_raagEngine.stepUp();
            else
                success = m_raagEngine.stepDown();
            if (!success)
                directionUp = !directionUp;
            numTries--;
        }

//        if (success)
//            DEBUG("StepUp");
//        else
//            DEBUG("Step Failed");
//        DEBUG("note: %s", note_map.at(m_m_raagEngine.getCurrentNote()).c_str());
//
        auto midi = m_raagEngine.getCurrentNoteAsMidi();
        DEBUG("midi: %d", midi);
        DEBUG("Octave: %d", m_raagEngine.getOctave());
//
//        auto freq = midiToFreq(midi);
//        DEBUG("freq: %f", freq);
//
//        auto volt = freqToVolt(freq);
//        DEBUG("volt: %f", volt);
//        DEBUG("----------");
    }

    // Set output V/OCT based on current note
    auto midi = m_raagEngine.getCurrentNoteAsMidi();
    auto freq = midiToFreq(midi - 60);
    auto volt = freqToVolt(freq);
    outputs[OUT_VOCT].setVoltage(volt);
}

void RaagSequencer::updateConnections() {
//    DEBUG("-----updateConnections------");
    auto& aroha = m_raagEngine.getAroha();
    auto& avroha = m_raagEngine.getAvroha();

    for (int i=0; i<12; i++) {
        // For Aroha
        if (inputs[IN_AROHA_SA + i].isConnected()) {
            auto volt = inputs[IN_AROHA_SA + i].getVoltage();
            if (volt != m_arohaInputLastValues[i]) {
                auto arohaFromNote = static_cast<Note>(volt * 12);
                auto arohaToNote = static_cast<Note>(i);
                DEBUG("\nVoltage: %f   Connect: %s to %s\n", volt, note_map.at(arohaFromNote).c_str(), note_map.at(arohaToNote).c_str());
                aroha.connect(arohaFromNote, arohaToNote);
                m_raagEngine.initLastNotes();
                m_arohaInputLastValues[i] = volt;
            }
        }
        else {
            if (m_arohaInputLastValues[i] != -1.f) {  // if note has not yet been disconnected from the graph
                auto arohaFromNote = static_cast<Note>(m_arohaInputLastValues[i] * 12);
                auto arohaToNote = static_cast<Note>(i);
                DEBUG("\nDisconnect: %s to %s\n", note_map.at(arohaFromNote).c_str(), note_map.at(arohaToNote).c_str());
                aroha.disconnect(arohaFromNote, arohaToNote);
                m_raagEngine.initLastNotes();
                m_arohaInputLastValues[i] = -1.f;
            }
        }

        // For Avroha
        if (inputs[IN_AVROHA_SA + i].isConnected()) {
            auto volt = inputs[IN_AVROHA_SA + i].getVoltage();
            if (volt != m_avrohaInputLastValues[i]) {
                auto avrohaFromNote = static_cast<Note>(volt * 12);
                auto avrohaToNote = static_cast<Note>(i);
                avroha.connect(avrohaFromNote, avrohaToNote);
                m_avrohaInputLastValues[i] = volt;
            }
        }
        else {
            if (m_avrohaInputLastValues[i] != -1.f) {  // -1 indicates that note has been disconnected from the graph
                auto avrohaFromNote = static_cast<Note>(m_avrohaInputLastValues[i] * 12);
                auto avrohaToNote = static_cast<Note>(i);
                avroha.disconnect(avrohaFromNote, avrohaToNote);
                m_avrohaInputLastValues[i] = -1.f;
            }
        }
    }
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
