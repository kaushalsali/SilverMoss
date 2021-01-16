//
// Created by Kaushal Sali on 07/12/20.
//

#ifndef MYPLUGIN_NOTEGRAPH_H
#define MYPLUGIN_NOTEGRAPH_H

//
// Created by Kaushal Sali on 04/12/20.
//

#include <map>
#include <unordered_set>

#include <iostream>
#include <unordered_map>
#include <sstream>

#include "plugin.hpp"

enum Note {Sa, re, Re, ga, Ga, Ma, ma, Pa, dha, Dha, ni, Ni, TOTAL, NONE};

/*
 * Hashing on enum class is not supported by all compilers. So we use this explicitly defined hashing functor which
 * does a static_cast before returning the hash value.
 */
struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

const std::unordered_map<Note, std::string, EnumClassHash> note_map {
        {Note::Sa, "Sa"}, {Note::re, "re"}, {Note::Re, "Re"},
        {Note::ga, "ga"}, {Note::Ga, "Ga"}, {Note::Ma, "Ma"},
        {Note::ma, "ma"}, {Note::Pa, "Pa"}, {Note::dha, "dha"},
        {Note::Dha, "Dha"}, {Note::ni, "ni"}, {Note::Ni, "Ni"}
};


using NotesSet = std::unordered_set<Note, EnumClassHash>;
using Graph = std::map<Note, NotesSet>;

/*
 * Builds a graph containing paths from a note to note. Each note can go to multiple unique notes.
 */
class NoteGraph {
public:
    NoteGraph();
    //TODO: Destructor needed ????

    const NotesSet & getConnectedNotes(Note from);
    void connect(Note from, Note to);
    void disconnect(Note from, Note to);
    void disconnectAll(Note from);
    void reset();

    Graph::const_iterator cbegin();
    Graph::const_iterator cend();
    Graph::const_reverse_iterator crbegin();
    Graph::const_reverse_iterator crend();

    std::string printGraph() { //TODO: Remove

        std::stringstream ss;
        ss << "------NoteGraph------" << std::endl;
        for(auto& noteConnections : m_graph) {
            ss << note_map.at(noteConnections.first) << " " << noteConnections.second.size() <<" : ";
            for(auto& nextNote : noteConnections.second) {
                ss << note_map.at(nextNote) << ", ";
            }
            ss << std::endl;
        }
        ss << "---------------------" << std::endl;
        return ss.str();
    }

private:
    Graph m_graph;
};

#endif //MYPLUGIN_NOTEGRAPH_H
