//
// Created by Kaushal Sali on 05/12/20.
//

#include "RaagEngine.h"


RaagEngine::RaagEngine() :
    m_lastNoteAroha(Note::NONE),
    m_lastNoteAvroha(Note::NONE)
{
    initLastNotes();
}

void RaagEngine::initLastNotes() {
    for (auto it = m_aroha.crbegin(); it != m_aroha.crend(); it++) {
        if (!it->second.empty()) {
            m_lastNoteAroha = it->first;
            break;
        }
    }
    for (auto it = m_avroha.cbegin(); it != m_avroha.cend(); it++) {
        if (!it->second.empty()) {
            m_lastNoteAvroha = it->first;
            break;
        }
    }
}

NoteGraph& RaagEngine::getAroha() {
    return m_aroha;
}

NoteGraph& RaagEngine::getAvroha() {
    return m_avroha;
}

Note RaagEngine::getCurrentNote() const {
    return m_currentNote;
}

int RaagEngine::getCurrentNoteAsMidi() const {
    return (m_octave+1) * 12 + static_cast<int>(m_currentNote) + m_transposition;
}

int RaagEngine::getTransposition() const {
    return m_transposition;
}

void RaagEngine::setTransposition(int semitone) {
    m_transposition = semitone;
}

int RaagEngine::getCurrentOctave() const {
    return m_octave;
}

void RaagEngine::setOctaveRange(int min, int max) {
    //TODO: This method should allow for setting octaves with offset (i.e. starting from arbitrary note instead of C)
    m_minOctave = min;
    m_maxOctave = std::max(min, max);   //TODO: Is this a good way to handle this?
    m_octave = std::max(std::min(m_octave, m_maxOctave), m_minOctave);
}

void RaagEngine::reset() {  //TODO: Complete. Clear RaagEngine objects ?
    m_minOctave = 3;
    m_maxOctave = 6;
    m_octave = 4;
    m_transposition = 0;
    m_currentNote = Note::Sa;
    initLastNotes();
}

bool RaagEngine::stepUp() {
    auto nextNotes = m_aroha.getConnectedNotes(m_currentNote);
    if (nextNotes.empty())
        return false;

    if(m_currentNote == m_lastNoteAroha)
        incOctave();
    randomGen.setRange(0, static_cast<int>(nextNotes.size())-1);
    auto offset = randomGen.generate();
    auto it = nextNotes.cbegin();
    std::advance(it, offset);    //TODO: This is expensive, linear complexity.
    m_currentNote = *it;
    return true;
}

bool RaagEngine::stepDown() {
    auto nextNotes = m_avroha.getConnectedNotes(m_currentNote);
    if (nextNotes.empty())
        return false;

    if(m_currentNote == m_lastNoteAvroha)
        decOctave();
    randomGen.setRange(0, static_cast<int>(nextNotes.size())-1);
    auto offset = randomGen.generate();
    auto it = nextNotes.cbegin();
    std::advance(it, offset);    //TODO: This is expensive, linear complexity.
    m_currentNote = *it;
    return true;
}

void RaagEngine::incOctave() {
    m_octave++;
    if (m_octave > m_maxOctave)
        m_octave = m_minOctave;
}

void RaagEngine::decOctave() {
    m_octave--;
    if (m_octave < m_minOctave)
        m_octave = m_maxOctave;
}

int RaagEngine::getMinOctave() const {
    return m_minOctave;
}

int RaagEngine::getMaxOctave() const {
    return m_maxOctave;
}

void RaagEngine::setCurrentOctave(int octave) {
    m_octave = std::max(std::min(octave, m_maxOctave), m_minOctave);
}
