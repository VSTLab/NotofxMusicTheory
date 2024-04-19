/*
 *  Diatonic.h
 *  MusicTheory
 *
 *  Created by Andreas Borg on 29/01/2013
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 
 ===============================================================================
 
 The diatonic module provides a simple interface for dealing with diatonic
 keys. The function get_notes(key) for instance returns the notes in a
 given key; even for extremely exotic notations ("C#####" or "Gbbbb").
 
 ================================================================================
 
 *
 */

#ifndef _Diatonic
#define _Diatonic

#include <iostream>

#include "Note.h"



namespace MusicTheory{
    
    static const std::string fifths = "F,C,G,D,A,E,B";
    static std::map<std::string, std::deque<NotePtr> > _keyCache;
    
class Diatonic {
	
  public:
	             
     
    
    
    /*
    Returns the note found at the interval starting from start_note 
    in the given key. For example interval('C', 'D', 1) will return 'E'. 
    Will raise a !KeyError if the start_note is not a valid note.
    */
    
    
    static NotePtr interval(NotePtr key, NotePtr startNote,int interval){
	
	    
        std::deque<NotePtr> notesInKey = getNotes(key);
    
        for(int n=0;n<notesInKey.size();n++){
            if(notesInKey[n]->name == startNote->name){
                return notesInKey[(n + interval) % 7];
            }
            
        }
   
      
    }

    
    
    /*
     Returns an ordered list of the notes in this key. \
     For example: if the key is set to 'F', this function will return \
     `['F', 'G', 'A', 'Bb', 'C', 'D', 'E']`. \
     Exotic or ridiculous keys like 'C####' or even 'Gbb##bb#b##' will work; \
     Note however that the latter example will also get cleaned up to 'G'. \
     This function will raise an !NoteFormatError if the key isn't recognised
     */
    static std::deque<NotePtr> getNotes(NotePtr key){
        //check cache
 
        if (_keyCache[key->name+std::to_string(key->octave)].size()>0){
        
            //since now shared ptrs need to return copies, else modifies map on use
            
            std::deque<NotePtr> copyDeque;
            for(NotePtr n:_keyCache[key->name+std::to_string(key->getOctave())]){
                copyDeque.push_back(n->copy());
            }
            
            return copyDeque;
        }
        
        
        
        //root note
        std::string root = key->name.substr(0,1);
        
        
        //fifth_index = 0 is a special case. It's the key of F and needs
        //Bb instead of B included in the result.
        
        auto fifthArr = utils::splitString(fifths, ",");
        std::vector<std::string>::iterator it = find(fifthArr.begin(),fifthArr.end(),root);
        int fifthIndex = it-fifthArr.begin();// Note::getNoteId(fifthArr,root);?
        
        std::deque<NotePtr> result;
        std::string keyAccidentals = key->name.substr(1);
        
        if(fifthIndex != 0){
            //not fifths
            //add
            std::string startNote = fifthArr[(fifthIndex - 1) % 7] + keyAccidentals;
            NotePtr note = Note::create(startNote,key->getOctave());
            result.push_back(note);
        
            for (int x=fifthIndex; x< fifthArr.size();x++){
                NotePtr note = Note::create(fifthArr[x]  +  keyAccidentals,key->getOctave());
                result.push_back(note);
            }
            
            for (int x=0; x< fifthIndex-1;x++){
                NotePtr note = Note::create(fifthArr[x]  + keyAccidentals+ "#",key->getOctave());
                result.push_back(note);
            }
            
        }else{
            //fifths
            for (int x=0; x< 6;x++){
                NotePtr note = Note::create(fifthArr[x] + keyAccidentals,key->getOctave());
                result.push_back(note);
            }
            NotePtr note = Note::create("Bb" + keyAccidentals,key->getOctave());
            result.push_back(note);
        }
        
        /*
        
        //this sorts all but doesn't start on key
        sort(result.begin(),result.end(),Note::comparePtr);

        
       // std::vector<Note>::iterator it = find(result.begin(),result.end(),root);
        int tonic = Note::getNoteId(result,key);
        
       // cout<<"tonic "<<tonic<<" "<<key<<" "<<result.size()<<endl;
        
        
        std::deque<NotePtr> keySorted;
        keySorted.insert(keySorted.begin(),result.begin()+tonic, result.end());
        
        //now in next octave
        for(int i=0;i<tonic;i++){
            result[i]->changeOctave(1);
            keySorted.push_back(result[i]);
        }
        */
        
        
        
        
        
        //set all to original octave
        int orgOct = key->getOctave();
        
        for(NotePtr note:result){
            note->setOctave(orgOct);
        }
        
        //this sorts all but doesn't start on key
        sort(result.begin(),result.end(),Note::comparePtr);

        //+1 octave if passed tonic
        int tonic = Note::getNoteId(result,key);
        std::deque<NotePtr> keySorted;
        if(tonic>-1){
            keySorted.insert(keySorted.begin(),result.begin()+tonic, result.end());

            //now in next octave
            for(int i=0;i<tonic;i++){
                //assuming scales within one octave
                result[i]->changeOctave(1);
                keySorted.push_back(result[i]);
            }
        }
#ifdef LOGS
        else
        {
            ofLogError()<<__FUNCTION__<<" Tonic not found"<< std::endl;
        }
#endif // LOGS

        //Save original to cache and return a copy so cache won't be corrupted by
        //modified pointers
        
        _keyCache[key->name+std::to_string(key->getOctave())] = keySorted;
        
        
        std::deque<NotePtr> copyDeque;
        for(NotePtr n:keySorted){
            copyDeque.push_back(n->copy());
        }
            
        return copyDeque;
    }
    
    
    
    static void print(std::deque<Note> notes){
        
        
        std::cout <<"[ ";
        for(int i = 0;i<notes.size();i++){
            std::cout<<notes[i];
            if(i<notes.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<< std::endl;
    }
    
    
    
    static void print(std::deque<NotePtr> notes){
        
        
        std::cout <<"[ ";
        for(int i = 0;i<notes.size();i++){
            std::cout<<notes[i];
            if(i<notes.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<< std::endl;
    }
    
    
		
};//class

}//namespace
#endif
