//
// Created by Kaushal Sali on 05/12/20.
//

#ifndef RAAGBUILDER_RAAGENGINE_H
#define RAAGBUILDER_RAAGENGINE_H

#include "NoteGraph.h"
#include "UniformRandomIntegerGenerator.h"

#include <array>

class RaagEngine {
public:
    RaagEngine();
    void initLastNotes();
    void reset();

    Note getCurrentNote() const;
    int getCurrentNoteAsMidi() const;
    int getTransposition() const;
    int getCurrentOctave() const;
    int getMinOctave() const;
    int getMaxOctave() const;
    void setCurrentNote(Note note);
    void setTransposition(int semitone);
    void setCurrentOctave(int octave);
    void setOctaveRange(int min, int max);
    void incOctave();
    void decOctave();

    void connectNotesInAroha(Note from, Note to);
    void connectNotesInAvroha(Note from, Note to);
    void disconnectNotesInAroha(Note from, Note to);
    void disconnectNotesInAvroha(Note from, Note to);
    void disconnectAllNotesInAroha(Note from);
    void disconnectAllNotesInAvroha(Note from);

    bool stepUp();
    bool stepDown();

    NoteGraph & getAroha() {return m_aroha;} // TODO: remove
    NoteGraph & getAvroha() {return m_avroha;} // TODO: remove

private:
    NoteGraph m_aroha;
    NoteGraph m_avroha;
    Note m_currentNote = Note::Sa;
    Note m_lastNoteAroha = Note::NONE;
    Note m_lastNoteAvroha = Note::NONE;
    std::array<int, static_cast<int>(Note::TOTAL)> m_notesInUseAroha;
    std::array<int, static_cast<int>(Note::TOTAL)> m_notesInUseAvroha;
    int m_transposition = 0;
    int m_currentOctave = 4;
    int m_minOctave = 3;
    int m_maxOctave = 6;
    UniformRandomIntegerGenerator randomGen;
};


#endif //RAAGBUILDER_RAAGENGINE_H
