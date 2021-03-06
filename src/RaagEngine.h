//
// Created by Kaushal Sali on 05/12/20.
//

#ifndef RAAGBUILDER_RAAGENGINE_H
#define RAAGBUILDER_RAAGENGINE_H

#include "NoteGraph.h"
#include "UniformRandomIntegerGenerator.h"

class RaagEngine {
public:
    RaagEngine();
    void initLastNotes();
    void reset();

    NoteGraph& getAroha();
    NoteGraph& getAvroha();

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

    bool stepUp();
    bool stepDown();

private:
    NoteGraph m_aroha;
    NoteGraph m_avroha;
    Note m_currentNote = Note::Sa;
    Note m_lastNoteAroha;
    Note m_lastNoteAvroha;
    int m_transposition = 0;
    int m_currentOctave = 4;
    int m_minOctave = 3;
    int m_maxOctave = 6;
    UniformRandomIntegerGenerator randomGen;
};


#endif //RAAGBUILDER_RAAGENGINE_H
