
#include "RaagSequencer.h"

RaagSequencer::RaagSequencer() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(PARAM_TRANSPOSE, -11.f, 11.f, 0.f, "Transpose");
    configParam(PARAM_OCTAVE_MIN, -2.f, 8.f, 4.f, "Octave Min");
    configParam(PARAM_OCTAVE_RANGE, 1.f, 10.f, 1.f, "Octave Range");
    configParam(PARAM_BACKTRACKING, 0.f, 1.f, 0.f, "Note Backtracking");
    configParam(PARAM_TRIGGER, 0.f, 1.f, 0.f, "Trigger");
    configParam(PARAM_DIRECTION, 0.f, 1.f, 1.f, "Direction");
    configParam(PARAM_RESET, 0.f, 1.f, 0.f, "Reset");
    configParam(PARAM_NUM_STEPS, 1.f, 12.f, 1.f, "Num Steps");

    for (int i=0; i<numArohaInputPorts; i++) {
        m_arohaInputLastNotes[i] = Note::NONE;
        m_avrohaInputLastNotes[i] = Note::NONE;
    }
    for (int i=0; i<numArohaOutputPorts; i++) { // Used for lights
        m_arohaNumInputConnections[i] = 0;
        m_avrohaNumInputConnections[i] = 0;
    }
    m_isFirstStep = true;
}

void RaagSequencer::process(const Module::ProcessArgs &args) {
    // Set voltage for output ports
    for (int i=0; i<12; i++) {
        outputs[OUT_AROHA_SA + i].setVoltage(static_cast<float>(i) / 12.0f);
        outputs[OUT_AVROHA_SA + i].setVoltage(static_cast<float>(i) / 12.0f);
    }

    // Update NoteGraph(s)
    if (!m_isFirstStep)
        updateConnections();
    else
        m_isFirstStep = false;

    // Reset
    // if button pressed or if received from CV port
    if (m_trigResetButton.process(params[PARAM_RESET].getValue()) || (inputs[IN_RESET].isConnected() && m_trigResetInput.process(inputs[IN_RESET].getVoltage()))) {
        // Turn on reset light
        m_resetLightBrightness = 1.f;
        lights[LIGHT_RESET].setBrightness(m_resetLightBrightness);

        auto currentNote = m_raagEngine.getCurrentNote();
        // Turn off red
        setArohaLightBrightness(currentNote, 0.f, 1);
        setAvrohaLightBrightness(currentNote, 0.f, 1);
        // Turn on green if connections exist
        if (m_arohaNumInputConnections[static_cast<int>(currentNote)] > 0)
            setArohaLightBrightness(currentNote, 1.f, 0);
        if (m_avrohaNumInputConnections[static_cast<int>(currentNote)] > 0)
            setAvrohaLightBrightness(currentNote, 1.f, 0);
        // Reset current Note
        currentNote = Note::Sa;
        m_raagEngine.setCurrentNote(currentNote);
        // Turn on red for updated note
        setArohaLightBrightness(currentNote, 1.f, 1);
        setAvrohaLightBrightness(currentNote, 1.f, 1);
    }

    // Trigger
    // if Trigger button pressed or received from CV port
    if (m_trigTriggerButton.process(params[PARAM_TRIGGER].getValue()) || (inputs[IN_TRIGGER].isConnected() && m_trigTriggerInput.process(inputs[IN_TRIGGER].getVoltage()))) {
        // Turn on reset light
        m_triggerLightBrightness = 1.f;
        lights[LIGHT_TRIGGER].setBrightness(m_triggerLightBrightness);

        // Set Transposition. This is done on trigger for performance
        auto transposeSemitone = static_cast<int>(params[PARAM_TRANSPOSE].getValue());
        if (transposeSemitone != m_raagEngine.getTransposition()) {
            m_raagEngine.setTransposition(transposeSemitone);
        }

        // Set octave ranges. This is done on trigger for performance
        auto octaveMin = static_cast<int>(params[PARAM_OCTAVE_MIN].getValue());
        auto octaveRange = static_cast<int>(params[PARAM_OCTAVE_RANGE].getValue());
        m_raagEngine.setOctaveRange(octaveMin, octaveMin + octaveRange);
        DEBUG("Octave----\nMin: %d Range: %d \nMin: %d Max: %d", octaveMin, octaveRange, m_raagEngine.getMinOctave(), m_raagEngine.getMaxOctave());

        // Set Num Steps. This is done on trigger for performance
        int numSteps;
        if (inputs[IN_NUM_STEPS].isConnected()) {
            auto volt = inputs[IN_NUM_STEPS].getVoltage();
            numSteps = std::min(std::max(1, static_cast<int>(volt / 10.f * 12.f) + 1), 12);; // map input voltage to range of 1 to 12
        }
        else
            numSteps = static_cast<int>(params[PARAM_NUM_STEPS].getValue());

        // Set Direction
        bool directionUp;
        if (inputs[IN_DIRECTION].isConnected())
            directionUp = inputs[IN_DIRECTION].getVoltage() >= 5.0f;
        else
            directionUp = params[PARAM_DIRECTION].getValue() == 1.f;

        // Set Backtracking
        auto backtracking = params[PARAM_BACKTRACKING].getValue() == 1.f;

        // Update lights before stepping
        auto currentNote = m_raagEngine.getCurrentNote();
        // Turn off red light
        setArohaLightBrightness(currentNote, 0.f, 1);
        setAvrohaLightBrightness(currentNote, 0.f, 1);
        // Turn on green light if at least one connection exists
        if (m_arohaNumInputConnections[static_cast<int>(currentNote)] > 0)
            setArohaLightBrightness(currentNote, 1.f, 0);
        if (m_avrohaNumInputConnections[static_cast<int>(currentNote)] > 0)
            setAvrohaLightBrightness(currentNote, 1.f, 0);


        // Step
        for (int i=0; i<numSteps; i++) {
            auto stepUp = directionUp;
            // If backtracking, and there's no connection in current direction, we try to take 1 step in the reverse direction
            //TODO: Improve logic. Perhaps an we could do more than one reverse steps ?? Think. Is this necessary?
            auto numTries = backtracking ? 2 : 1;
            auto success = false;
            while (!success && numTries) {
                if (stepUp)
                    success = m_raagEngine.stepUp();
                else
                    success = m_raagEngine.stepDown();
                if (!success)
                    stepUp = !stepUp;
                numTries--;
            }
        }

        // Update lights after stepping to new note
        currentNote = m_raagEngine.getCurrentNote();
        // Turn off green light
        setArohaLightBrightness(currentNote, 0.f, 0);
        setAvrohaLightBrightness(currentNote, 0.f, 0);
        // Turn on red light
        setArohaLightBrightness(currentNote, 1.f, 1);
        setAvrohaLightBrightness(currentNote, 1.f, 1);


//        DEBUG("\n-----------Aroha\"-----------\n%s", m_raagEngine.getAroha().printGraph().c_str());
//        DEBUG("\n-----------Avroha\"-----------\n%s", m_raagEngine.getAvroha().printGraph().c_str());
//        DEBUG("Direction: %i", static_cast<int>(directionUp));
//        DEBUG("Note Played: %s", note_map.at(m_raagEngine.getCurrentNote()).c_str());
//        DEBUG("Octave\nMin: %d %d\nMax: %d %d\nCurrent: %d", octaveMin, m_raagEngine.getMinOctave(), octaveMax, m_raagEngine.getMaxOctave(), m_raagEngine.getCurrentOctave());
//        DEBUG("Transposition: %d", m_raagEngine.getTransposition());
//        auto midi = m_raagEngine.getCurrentNoteAsMidi();
//        DEBUG("Midi: %d", midi);
//        DEBUG("Octave: %d", m_raagEngine.getOctave());
//        DEBUG("-------------");
//        auto freq = midiToFreq(midi);
//        DEBUG("freq: %f", freq);
//        auto volt = freqToVolt(freq);
//        DEBUG("volt: %f", volt);
//        DEBUG("----------");
    }

    // Handle lights // TODO: Optimize. Do this every frame.
    // Dim reset light
    if (m_resetLightBrightness > 0.f) {
        auto decayAmount = 0.0001;
        m_resetLightBrightness -= decayAmount;
        lights[LIGHT_RESET].setBrightness(m_resetLightBrightness); //TODO: Use smoothBrightness() ??
    }
    // Dim Trigger light
    if (m_triggerLightBrightness > 0.f) {
        auto decayAmount = 0.0001;
        m_triggerLightBrightness -= decayAmount;
        lights[LIGHT_TRIGGER].setBrightness(m_triggerLightBrightness); //TODO: Use smoothBrightness() ??
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

    for (int i=0; i<numArohaInputPorts; i++) {
        // For Aroha
        if (inputs[IN_AROHA_SA + i].isConnected()) {
//            DEBUG("\n--------here--1----------------\n");
            auto volt = inputs[IN_AROHA_SA + i].getVoltage();
            //TODO: Decide whether to wrap or limit voltage
            //volt = volt - static_cast<int>(volt);   // wrap to range 0V to 0.916V
            volt = std::min(std::max(0.f, volt), static_cast<float>(Note::Ni)/12); // limit range from 0V to 0.916V
            auto arohaFromNote = static_cast<Note>(static_cast<int>(volt * 12));
            //TODO: Handle voltage to Note conversion more elegantly

            if (arohaFromNote != m_arohaInputLastNotes[i]) {
                auto arohaToNote = static_cast<Note>(i%12);
                if (m_arohaInputLastNotes[i] != Note::NONE) {  // Disconnect previous note if present
                    auto arohaOldFromNote = static_cast<Note>(m_arohaInputLastNotes[i]);
                    aroha.disconnect(arohaOldFromNote, arohaToNote);
                    m_arohaNumInputConnections[i%12]--;
//                    DEBUG("\nDisconnect: %s to %s\n", note_map.at(arohaOldFromNote).c_str(), note_map.at(arohaToNote).c_str());
                }
//                DEBUG("\nVoltage: %d   Connect: %s to %s\n", volt, note_map.at(arohaNewFromNote).c_str(), note_map.at(arohaToNote).c_str());
                aroha.connect(arohaFromNote, arohaToNote);
                m_raagEngine.initLastNotes();
                m_arohaInputLastNotes[i] = arohaFromNote;
                m_arohaNumInputConnections[i%12]++;
                setArohaLightBrightness(arohaToNote, 1.f, 0);
            }
        }
        else {
            if (m_arohaInputLastNotes[i] != Note::NONE) {  // if note has not yet been disconnected from the graph
                auto arohaFromNote = static_cast<Note>(m_arohaInputLastNotes[i]);
                auto arohaToNote = static_cast<Note>(i%12);
                aroha.disconnect(arohaFromNote, arohaToNote);
                m_raagEngine.initLastNotes();
                m_arohaInputLastNotes[i] = Note::NONE;
                m_arohaNumInputConnections[i%12]--;
                if (m_arohaNumInputConnections[i%12] == 0)
                    setArohaLightBrightness(arohaToNote, 0.f, 0);
//                DEBUG("\nDisconnect: %s to %s\n", note_map.at(arohaFromNote).c_str(), note_map.at(arohaToNote).c_str());
            }
        }
        // For Avroha
        if (inputs[IN_AVROHA_SA + i].isConnected()) {
            auto volt = inputs[IN_AVROHA_SA + i].getVoltage();
            // volt = volt - static_cast<int>(volt);   // wrap to range 0V to 1V
            volt = std::min(std::max(0.f, volt), static_cast<float>(Note::Ni)/12); // limit range from 0V to 0.916V
            auto avrohaFromNote = static_cast<Note>(static_cast<int>(volt * 12));
            //TODO: Handle voltage to Note conversion more elegantly

            if (avrohaFromNote != m_avrohaInputLastNotes[i]) {
                auto avrohaToNote = static_cast<Note>(i%12);
                if (m_avrohaInputLastNotes[i] != Note::NONE) {  // Disconnect previous note if present
                    auto avrohaOldFromNote = static_cast<Note>(m_avrohaInputLastNotes[i]);
                    avroha.disconnect(avrohaOldFromNote, avrohaToNote);
                    m_avrohaNumInputConnections[i%12]--;
                }
                avroha.connect(avrohaFromNote, avrohaToNote);
                m_raagEngine.initLastNotes();
                m_avrohaInputLastNotes[i] = avrohaFromNote;
                m_avrohaNumInputConnections[i%12]++;
                setAvrohaLightBrightness(avrohaToNote, 1.f, 0);
            }
        }
        else {
            if (m_avrohaInputLastNotes[i] != Note::NONE) {  // -1 indicates that note has been disconnected from the graph
                auto avrohaFromNote = static_cast<Note>(m_avrohaInputLastNotes[i]);
                auto avrohaToNote = static_cast<Note>(i%12);
                avroha.disconnect(avrohaFromNote, avrohaToNote);
                m_raagEngine.initLastNotes();
                m_avrohaInputLastNotes[i] = Note::NONE;
                m_avrohaNumInputConnections[i%12]--;
                if (m_avrohaNumInputConnections[i%12] == 0)
                    setAvrohaLightBrightness(avrohaToNote, 0.f, 0);
            }
        }
    }
}

void RaagSequencer::setArohaLightBrightness(Note note, float brightness, int colorIndex /* = 0 */) {
    lights[LIGHT_AROHA_SA + static_cast<int>(note) * numLightColors + colorIndex].setBrightness(brightness);
}

void RaagSequencer::setAvrohaLightBrightness(Note note, float brightness, int colorIndex /* = 0 */) {
    lights[LIGHT_AVROHA_SA + static_cast<int>(note) * numLightColors + colorIndex].setBrightness(brightness);
}

float RaagSequencer::getArohaLightBrightness(Note note, int colorIndex) {
    return lights[LIGHT_AROHA_SA + static_cast<int>(note) * numLightColors + colorIndex].getBrightness();
}

float RaagSequencer::getAvrohaLightBrightness(Note note, int colorIndex) {
    return lights[LIGHT_AVROHA_SA + static_cast<int>(note) * numLightColors + colorIndex].getBrightness();
}

RaagSequencerWidget::RaagSequencerWidget(RaagSequencer* module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RaagSequencer.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    for (int i=0; i<Note::TOTAL; i++) {
        // Aroha
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15, panelHeight - (10.f + i * 10))), module, RaagSequencer::IN_AROHA_SA + i + 12));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30, panelHeight - (10.f + i * 10))), module, RaagSequencer::IN_AROHA_SA + i));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45, panelHeight - (10.f + i * 10))), module, RaagSequencer::OUT_AROHA_SA + i));
        addChild(createLightCentered<MediumLight<GreenRedLight>>(mm2px(Vec(53, panelHeight - (10.f + i * 10))), module, RaagSequencer::LIGHT_AROHA_SA + i * RaagSequencer::numLightColors));
        // Avroha
        addChild(createLightCentered<MediumLight<GreenRedLight>>(mm2px(Vec(panelWidth - 53, panelHeight - (10.f + i * 10))), module, RaagSequencer::LIGHT_AVROHA_SA + i * RaagSequencer::numLightColors));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(panelWidth - 45, panelHeight - (10.f + i * 10))), module, RaagSequencer::OUT_AVROHA_SA + i));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth - 30, panelHeight - (10.f + i * 10))), module, RaagSequencer::IN_AVROHA_SA + i));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth - 15, panelHeight - (10.f + i * 10))), module, RaagSequencer::IN_AVROHA_SA + i + 12));
    }

    // Control Section
    // Octave Range
    addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(panelWidth/2, 10 + 0.5 * 10)), module, RaagSequencer::PARAM_TRANSPOSE));
    addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(panelWidth/2 - 8, 10 + 2 * 10)), module, RaagSequencer::PARAM_OCTAVE_MIN));
    // Transpose
    addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(panelWidth/2 + 8, 10 + 2 * 10)), module, RaagSequencer::PARAM_OCTAVE_RANGE));
    // Backtracking
    addParam(createParamCentered<CKSS>(mm2px(Vec(panelWidth/2, 10 + 3.5 * 10)), module, RaagSequencer::PARAM_BACKTRACKING));
    // Trigger
    addParam(createParamCentered<LEDBezel>(mm2px(Vec(panelWidth/2 - 10, 10 + 4.5 * 10)), module, RaagSequencer::PARAM_TRIGGER));
    addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(panelWidth/2 - 10, 10 + 4.5 * 10)), module, RaagSequencer::LIGHT_TRIGGER));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth/2 + 10, 10 + 4.5 * 10)), module, RaagSequencer::IN_TRIGGER));
    // Direction
    addParam(createParamCentered<CKSS>(mm2px(Vec(panelWidth/2 - 10, 10 + 6 * 10)), module, RaagSequencer::PARAM_DIRECTION));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth/2 + 10, 10 + 6 * 10)), module, RaagSequencer::IN_DIRECTION));
    // Reset
    addParam(createParamCentered<LEDBezel>(mm2px(Vec(panelWidth/2 - 10, 10 + 7.5 * 10)), module, RaagSequencer::PARAM_RESET));
    addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(panelWidth/2 - 10, 10 + 7.5 * 10)), module, RaagSequencer::LIGHT_RESET));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth/2 + 10, 10 + 7.5 * 10)), module, RaagSequencer::IN_RESET));
    // Num Steps
    addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(panelWidth/2 - 10, 10 + 9 * 10)), module, RaagSequencer::PARAM_NUM_STEPS));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(panelWidth/2 + 10, 10 + 9 * 10)), module, RaagSequencer::IN_NUM_STEPS));
    // V/OCT
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(panelWidth/2, 10 + 10.5 * 10)), module, RaagSequencer::OUT_VOCT));
}
