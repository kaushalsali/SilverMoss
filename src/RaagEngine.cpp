//
// Created by Kaushal Sali on 05/12/20.
//

#include "RaagEngine.h"
#include "plugin.hpp"

RaagEngine::RaagEngine() :
    m_notesInUseAroha(),
    m_notesInUseAvroha()
{
    initLastNotes();
    for (int i=0; i<12; i++) {
        DEBUG("--%d", m_notesInUseAroha[i]);
    }

}

void RaagEngine::initLastNotes() {
    m_lastNoteAroha = Note::NONE;
    for (auto i = static_cast<int>(Note::Ni); i >= static_cast<int>(Note::Sa); i--) {
        if (m_notesInUseAroha[i] > 0) {  // will be > 0 if there is a 'from' or 'to' connection.
            m_lastNoteAroha = static_cast<Note> (i);
            break;
        }
    }
    m_lastNoteAvroha = Note::NONE;
    for (auto i = static_cast<int>(Note::Sa); i <= static_cast<int>(Note::Ni); i++) {
        if (m_notesInUseAvroha[i] > 0) {  // will be > 0 if there is a 'from' or 'to' connection.
            m_lastNoteAvroha = static_cast<Note> (i);
            break;
        }
    }
    DEBUG("Aroha last: %s", note_map.at(m_lastNoteAroha).c_str());
    DEBUG("Avroha last: %s", note_map.at(m_lastNoteAvroha).c_str());

    for (int i=0; i<12; i++) {
        DEBUG("%d", m_notesInUseAroha[i]);
    }
    DEBUG("--------" );


}

Note RaagEngine::getCurrentNote() const {
    return m_currentNote;
}

int RaagEngine::getCurrentNoteAsMidi() const {
    return (m_currentOctave+1) * 12 + static_cast<int>(m_currentNote) + m_transposition;
}

int RaagEngine::getTransposition() const {
    return m_transposition;
}

void RaagEngine::setTransposition(int semitone) {
    m_transposition = semitone;
}

int RaagEngine::getCurrentOctave() const {
    return m_currentOctave;
}

void RaagEngine::setOctaveRange(int min, int max) {
    //TODO: This method should allow for setting octaves with offset (i.e. starting from arbitrary note instead of C)
    m_minOctave = min;
    m_maxOctave = std::max(min, max);   //TODO: Is this a good way to handle this?
    m_currentOctave = std::max(std::min(m_currentOctave, m_maxOctave), m_minOctave);
}

void RaagEngine::reset() {
    m_minOctave = 3;
    m_maxOctave = 6;
    m_currentOctave = 4;
    m_transposition = 0;
    m_currentNote = Note::Sa;
    m_aroha.reset();
    m_avroha.reset();
    initLastNotes();
}

bool RaagEngine::stepUp() {
    auto nextNotes = m_aroha.getConnectedNotes(m_currentNote);
    if (nextNotes.empty())
        return false;

    if(m_currentNote == m_lastNoteAroha)
        incOctave();

    randomGen.setRange(0, static_cast<int>(nextNotes.size())-1);
    auto randomIdx = randomGen.generate();
    m_currentNote = nextNotes[randomIdx];
    return true;
}

bool RaagEngine::stepDown() {
    auto nextNotes = m_avroha.getConnectedNotes(m_currentNote);
    if (nextNotes.empty())
        return false;

    if(m_currentNote == m_lastNoteAvroha)
        decOctave();

    randomGen.setRange(0, static_cast<int>(nextNotes.size())-1);
    auto randomIdx = randomGen.generate();
    m_currentNote = nextNotes[randomIdx];
    return true;
}

void RaagEngine::incOctave() {
    m_currentOctave++;
    if (m_currentOctave > m_maxOctave)
        m_currentOctave = m_minOctave;
}

void RaagEngine::decOctave() {
    m_currentOctave--;
    if (m_currentOctave < m_minOctave)
        m_currentOctave = m_maxOctave;
}

int RaagEngine::getMinOctave() const {
    return m_minOctave;
}

int RaagEngine::getMaxOctave() const {
    return m_maxOctave;
}

void RaagEngine::setCurrentOctave(int octave) {
    m_currentOctave = std::max(std::min(octave, m_maxOctave), m_minOctave);
}

void RaagEngine::setCurrentNote(Note note) {
    m_currentNote = note;
}

void RaagEngine::connectNotesInAroha(Note from, Note to) {
    m_aroha.connect(from, to);
    m_notesInUseAroha[static_cast<int>(from)]++;
    m_notesInUseAroha[static_cast<int>(to)]++;
}

void RaagEngine::connectNotesInAvroha(Note from, Note to) {
    m_avroha.connect(from, to);
    m_notesInUseAvroha[static_cast<int>(from)]++;
    m_notesInUseAvroha[static_cast<int>(to)]++;
}
void RaagEngine::disconnectNotesInAroha(Note from, Note to) {
    m_aroha.disconnect(from, to);
    m_notesInUseAroha[static_cast<int>(from)]--;
    m_notesInUseAroha[static_cast<int>(to)]--;
}
void RaagEngine::disconnectNotesInAvroha(Note from, Note to) {
    m_avroha.disconnect(from, to);
    m_notesInUseAvroha[static_cast<int>(from)]--;
    m_notesInUseAvroha[static_cast<int>(to)]--;
}

void RaagEngine::disconnectAllNotesInAroha(Note from) {

}

void RaagEngine::disconnectAllNotesInAvroha(Note from) {

}
