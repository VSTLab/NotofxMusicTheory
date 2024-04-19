#include "MusicTheory/MusicTheory.h"
#include <iostream>
#include <deque>
#include <vector>
#include <string>

using namespace MusicTheory;
//--------------------------------------------------------------
int main(){
    NotePtr C = Note::create("C");
    std::cout<<C<< std::endl;
    
    NotePtr C2 = Note::fromInt(60);
    std::cout<<C2<<std::endl;
    
    NotePtr b = C->getAugmented();
    
    C->augment();
    std::cout<<"Aug "<<C<<std::endl;
    
    C->diminish();
    std::cout<<"Dim "<<C<<std::endl;
    
    C->diminish();
    std::cout<<"Dim "<<C<<std::endl;
    
    
    b->changeOctave(-1);
    b->augment();
    b->augment();
    b->augment();
    std::cout<<b<<std::endl;
    
    
    std::cout<<"Diatonic name: "<<b->getDiatonicName()<<std::endl;
    
    NotePtr c = b->getOctaveDown();
    std::cout<<"Oct down: "<<c<<std::endl;
    
    NotePtr r = Note::create("C");
   
    std::cout<<"Get diatones of: "<<r<<std::endl;
    
    std::deque<NotePtr> scale = Diatonic::getNotes(r);
     
    for(int i=0;i<scale.size();i++){
        std::cout<<scale[i]<<std::endl;
    }

    
    NotePtr root = Note::create("C");
    
    
    NotePtr abs6 = Interval::sixth(root,root);
    std::cout<<"Absolute sixth of "<<root<<" is "<<abs6<<std::endl;
    
    
    
    NotePtr abs4 = Interval::sixth(root,root);
    
        
    NotePtr m2 = Interval::minorSecond(root);
    std::cout<<"Min second of "<<root<<" is "<<m2<<std::endl;
    
    NotePtr M2 = Interval::majorSecond(root);
    std::cout<<"Maj second of "<<root<<" is "<<M2<<std::endl;
    
    
    NotePtr m3 = Interval::minorThird(root);
    std::cout<<"Min third of "<<root<<" is "<<m3<<std::endl;
    
    NotePtr M3 = Interval::majorThird(root);
    std::cout<<"Maj third of "<<root<<" is "<<M3<<std::endl;
    
    
    NotePtr m4 = Interval::minorFourth(root);
    std::cout<<"Min fourth of "<<root<<" is "<<m4<<std::endl;
    
    NotePtr P4 = Interval::perfectFourth(root);
    std::cout<<"Perfect fourth of "<<root<<" is "<<P4<<std::endl;
    
    NotePtr M4 = Interval::majorFourth(root);
    std::cout<<"Maj fourth of "<<root<<" is "<<M4<<std::endl;
    
    
    NotePtr m5 = Interval::minorFifth(root);
    std::cout<<"Min fifth of "<<root<<" is "<<m5<<std::endl;
    
    NotePtr P5 = Interval::perfectFifth(root);
    std::cout<<"Perfect fifth of "<<root<<" is "<<P5<<std::endl;
    
    NotePtr m6 = Interval::minorSixth(root);
    std::cout<<"Min sixth of "<<root<<" is "<<m6<<std::endl;
    
    NotePtr M6 = Interval::majorSixth(root);
    std::cout<<"Maj sixth of "<<root<<" is "<<M6<<std::endl;
    
    NotePtr m7 = Interval::minorSeventh(root);
    std::cout<<"Min seven of "<<root<<" is "<<m7<<std::endl;

    NotePtr M7 = Interval::majorSeventh(root);
    std::cout<<"Maj seven of "<<root<<" is "<<M7<<std::endl;
    
    
    
    std::string m = Chord::getFullName("mM7");
    
    std::cout<<Chord::getFullName("mM7")<<" This one not found:" <<Chord::getFullName("oddname")<<std::endl;
    //ofLog()<<a->name<<std::endl;
    
    
    ChordPtr ch = Chord::halfDiminishedSeventh(root);
    std::cout<<ch<<std::endl;;
    
    
    ChordPtr st = Chord::tonic7(root);
    std::cout<<"Subtonic to "<<root<<" is "<<st<<std::endl;
    
    
    ChordPtr subdom = Chord::subdominant(root);
    std::cout<<"Subdominant to "<<root<<" is "<<subdom<<std::endl;
    
    
    ChordPtr subdomInv = Chord::firstInversion(subdom);
    std::cout<<"Subdominant first inversion is "<<subdomInv<<std::endl;
    
    
    /*
     Extended fifth chords are made of built of 5 notes at third dist
     */
    
    ChordPtr ext5 = Chord::majorNinth(root);
    std::cout<<"Ext fifth eg."<<ext5<<std::endl;
    
    

    bool useShorthand = true;
    bool allowInvensions = true;
    bool allowPolychords = true;
    
    std::deque<NotePtr> triad = {
        {Note::create("F")},
        {Note::create("G")},
        {Note::create("C")}
    };
    
    std::cout<<"Triad analysis "<<std::endl;
    Chord::print(triad);
    std::vector<std::string> analyse = Chord::analyse(triad, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }
    
    
    
    std::deque<NotePtr> seventh = {
        {Note::create("C")},
        {Note::create("E")},
        {Note::create("G")},
        {Note::create("A")}
    };
    
    std::cout<<std::endl<<"Seventh analysis "<<std::endl;
    
    Chord::print(seventh);
    
    analyse = Chord::analyse(seventh, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }
    
    
    std::deque<NotePtr> ext = {
        {Note::create("C")},
        {Note::create("E")},
        {Note::create("G")},
        {Note::create("B")},
        {Note::create("D")}
    };

    std::cout<<std::endl<<"Extended  fifth analysis "<<std::endl;
    Chord::print(ext);
    analyse = Chord::analyse(ext, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }
    
    
    std::cout<<std::endl<<"Extended  sixth analysis "<<std::endl;
    ext.push_back(Note::create("A"));
    Chord::print(ext);
    analyse = Chord::analyse(ext, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }
    
    

    
    
    std::cout<<std::endl<<"Extended  seventh analysis "<<std::endl;
    
    ChordPtr ext7 = Chord::majorNinth(root);
    
    //add 11 and 13 to the 9th
    ext7->notes.push_back(Interval::fourth(root, root));
    ext7->notes.push_back(Interval::sixth(root, root));
    
    //std::cout<<"Ext seventh eg."<<ext7<<std::endl;
    
    //turn vector into deque...(using deque since internally we need to shuffle aruond to test different inversions
    std::deque<NotePtr> ch13;
    ch13.insert(ch13.begin(),ext7->notes.begin(),ext7->notes.end());
    
    Chord::print(ch13);
    
    analyse = Chord::analyse(ch13, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }

    
    
    
    allowPolychords = true;
    
    
    std::cout<<std::endl<<"Polychord analysis"<<std::endl;
    
    std::deque<NotePtr> poly = Chord::triad(Note::create("C"),Note::create("C"))->notes;
    
    std::deque<NotePtr> chord2= Chord::minorTriad(Note::create("F#"))->notes;
    
    poly.insert(poly.end(), chord2.begin(),chord2.end());
    
    
    Chord::print(poly);
    
    analyse = Chord::analyse(poly, useShorthand,allowInvensions,allowPolychords);
    for(int i = 0;i<analyse.size();i++){
        std::cout<<"Opt "<<i<<": "<<analyse[i]<<std::endl;
    }
    
    
    
    std::cout<<std::endl<<"Scales"<<std::endl;
    ScalePtr lyd = Scale::getLydian(Note::create("F"));
    std::cout<<lyd<<std::endl;
    


    ScalePtr melMin = Scale::getMelodicMinor(Note::create("C"));
    std::cout<<melMin<<std::endl;
    
    ScalePtr pentaMaj = Scale::getPentatonicMajor(Note::create("C"));
    std::cout<<pentaMaj<<std::endl;
    
    
    ScalePtr hirajoshi = Scale::getHirajoshi(Note::create("C"));
    std::cout<<hirajoshi<<std::endl;
    
    
    std::cout<<"V7 to "<<root<<" is ";
    ChordPtr dom7= Progression::getChordFromRoman("V7",root);
    std::cout<<dom7<<std::endl;
    
    std::cout<<"VIdim7 to "<<root<<" is ";
  
    
    
    
    ChordPtr pchord = Progression::getChordfromChordFunction("VIdim7",root);//add dim etc
    std::cout<<pchord<<std::endl;
    
    
    std::string progStr = "bIIdim7,VM7";
    std::deque<ChordPtr> prog = Progression::fromString(progStr,root);
    
    std::cout<<progStr<<" where I is "<<root<<std::endl;
    Progression::print(prog);
    
    
    std::string subtituteMe = "IV7";
    std::cout<<std::endl<<"Harmonic substitution of "<<subtituteMe<<" ";
    Progression::print(Progression::substituteHarmonic(subtituteMe));
    
    
    subtituteMe = "VIm7";
    std::cout<<std::endl<<"Minor for major substitution of "<<subtituteMe<<" ";
    Progression::print(Progression::substituteMinorForMajor(subtituteMe));
    
    
    subtituteMe = "VM7";
    std::cout<<std::endl<<"Major for minor substitution of "<<subtituteMe<<" ";
    Progression::print(Progression::substituteMajorForMinor(subtituteMe));

    subtituteMe = "VII";
    std::cout<<std::endl<<"Dim for dim substitution of "<<subtituteMe<<" ";
    Progression::print(Progression::substituteDiminishedForDiminished(subtituteMe));

    
    subtituteMe = "VIIdim";
    std::cout<<std::endl<<"Dim for dom substitution of "<<subtituteMe<<" ";
    Progression::print(Progression::substituteDiminishedForDominant(subtituteMe));
    
    
  
    //NotePtr root("E",7);
    
    
    
    ChordPtr I = Progression::getChordfromChordFunction("I",root);
    std::cout<<I<<std::endl;
    
    ChordPtr II = Progression::getChordfromChordFunction("II",root);
    std::cout<<II<<std::endl;
    
    
    ChordPtr IV = Progression::getChordfromChordFunction("IVdim",root);
    std::cout<<IV<<std::endl;

    ChordPtr V7 = Progression::getChordfromChordFunction("V7",root);
    std::cout<<V7<<std::endl;
    
    std::cout<<root->name<<" ----------------------------"<<std::endl;
    
    ScalePtr mixo = Scale::getMixolydian(Note::create("C",6));
    std::cout<<mixo<<std::endl;
    
    ScalePtr prygian = Scale::getPhrygian(Note::create("C",6));
    std::cout<<prygian<<std::endl;

    
    ScalePtr flamenco = Scale::getFlamenco(Note::create("E",6));
    std::cout<<flamenco<<std::endl;
    
    ScalePtr Zen =  Scale::getInSen(Note::create("C",6));
    
    std::cout<<"Scales for 7b9b5"<<std::endl;
    std::vector<std::string>scales = Scale::getScalesForChord("7b9b5");
    Scale::print(scales);
    
    
    //NotePtr nn  = Interval::fromShorthand(Note::create("C",6), "b3");
    //std::cout<<nn<<std::endl;
    
    melMin = Scale::getScaleFromString(scales[0], Note::create("C",6));
    std::cout<<melMin<<std::endl;
    

    
}
