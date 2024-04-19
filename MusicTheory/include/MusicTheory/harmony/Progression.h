/*
 *  Progression.h
 *  MusicTheory
 *
 *  Created by Andreas Borg on 07/02/2013
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Progression
#define _Progression

#include <boost/regex.hpp>

#include "Chord.h"


namespace MusicTheory{

    static std::string ROMAN[] = {"I","II","III","IV","V","VI","VII"};
    static int numeral_intervals[] = {0, 2, 4, 5, 7, 9, 11};

    typedef std::map<std::string, int> IntLookup;
    
    static IntLookup RomanLookup = {
        {"I",0},
        {"II",1},
        {"III",2},
        {"IV",3},
        {"V",4},
        {"VI",5},
        {"VII",6},
    
    };
    
    typedef ChordPtr (*ChordFunctionPointer)(NotePtr);
    typedef std::map<std::string,ChordFunctionPointer> ChordFunctionLookup;
    
    static ChordFunctionLookup ChordFunctions ={
        {"I", &Chord::I},
        {"IM7", &Chord::IM7},
        {"I7", &Chord::I7},
        {"II", &Chord::II},
        {"II7", &Chord::II7},
        {"III", &Chord::III},
        {"III7", &Chord::III7},
        {"IV", &Chord::IV},
        {"IV7", &Chord::IV7},
        {"V", &Chord::V},
        {"V7", &Chord::V7},
        {"VI", &Chord::VI},
        {"VI7", &Chord::VI7},
        {"bVII", &Chord::bVII},
        {"VII", &Chord::VII},
        {"bVII7", &Chord::bVII7},
        {"VII7", &Chord::VII7}
    };

    
    
    
    typedef struct ChordTuple{
        std::string roman = "I";//eg. V
        //std::string bass;
        //std::string poly;
        int numeral = 0;//roman converted
        int accidentals = 0;//eg. # = +1
        std::string cleanedAccidentals = "";//reduced ##b to #
        std::string suffix = "";//eg. dim7
    }ChordTuple;
    
class Progression : public std::enable_shared_from_this<Progression> {
	
  public:
	
	Progression(){};
	    
    
    /*
    static shared_ptr<Progression>create(){
        return shared_ptr<Progression>(new Progression());
    }
    
    shared_ptr<Progression> copy(){
        return shared_ptr<Progression>(new Progression(*this));//copy
    }
    */
    

   // numeralInterval = [0, 2, 4, 5, 7, 9, 11]
    
    
    
    /*
     Converts a list of chord functions (eg `['I', 'V7']`) or 
     a std::string (eg. 'I7') to a list of chords. 
     Any number of accidentals can be used as prefix to augment or diminish; 
     for example: bIV or #I. All the chord abbreviations in the chord module 
     can be used as suffixes; for example: Im7, IVdim7, etc. 
     You can combine prefixes and suffixes to manage complex progressions: 
     #vii7, #iidim7, iii7, etc. 
     Using 7 as suffix is ambiguous, since it is classicly used to denote 
     the seventh chord when talking about progressions instead of _just_ the 
     dominanth seventh chord. We have taken the classic route; I7 
     will get you a major seventh chord. If you specifically want a dominanth 
     seventh, use Idom7.
     
     Note: This is not followed in my implementation. I'm too jazzy for that.
     */
    static std::deque<ChordPtr> fromString(std::string progression, NotePtr key = Note::create("C")){
        std::vector<std::string> prog = utils::splitString(progression,",");
    
        std::deque<ChordPtr> result;
        
        ChordPtr chord;
        
        for (int c=0;c<prog.size();c++){
            std::string chordStr = prog[c];
            chord = Progression::getChordfromChordFunction(chordStr,key);
            result.push_back(chord);
        }
        
        return result;
    }
	
    
    static ChordPtr getChordfromChordFunction(std::string chordFunction, NotePtr key = Note::create("C")){
        //strip preceding accidentals from the std::string
        std::vector<std::string> slash = utils::splitString(chordFunction,"/");
        std::vector<std::string> poly = utils::splitString(chordFunction,"| ");
        
        std::string chordTop;
        std::string bass="";
        std::string chordBottom="";
        
        
        bool isSlash = Chord::isSlashChord(chordFunction);
        //TODO: Test slash chords for romans
        if(isSlash){
            chordTop = slash[0];
            bass = slash[1];
        }else if(poly.size()==2){
            chordTop = poly[0];
            chordBottom = poly[1];
        }else{
            chordTop = chordFunction;
            
        }
        
        ChordTuple tuple = Progression::parse(chordTop);
        ChordPtr chord = Progression::tupleToChord(tuple,key);
       
         if(isSlash){
            ChordTuple bassTuple = Progression::parse(bass);
            ChordPtr bassNote = Progression::tupleToChord(bassTuple,key);
            chord->setBass(bassNote->notes[0]);
            
        }else if(poly.size()==2){
            ChordTuple polyTuple = Progression::parse(chordBottom);
            ChordPtr polyChord = Progression::tupleToChord(polyTuple,key);
            //I don't know if anything is missing here...for subchord symbols etc
            /*
            Chord subchord;
            
            subchord.name = chordSymbol;
            subchord.notes = chord.notes;
            subchord.setRoot(note);*/
            
            chord->setPolyChord(polyChord);
        }
        
        return chord;
        //These suffixes don't need any post processing
       
    }
    
    /*
     Doesn't work?
     */
    static std::vector<std::string> getFunctionFromChord(ChordPtr chord, NotePtr key = Note::create("C")){
        std::vector<std::string> prog;
        if(!chord)
        {
#ifdef LOGS
            ofLogError()<<"Progression::getFunctionFromChord failed. No chord"<<std::endl;
#endif // LOGS
            return prog;
        }
        if(!chord->notes.size())
        {
#ifdef LOGS
            ofLogError()<<"Progression::getFunctionFromChord failed. No chord notes"<<std::endl;
#endif // LOGS
            return prog;
        }
        prog = Progression::determine(chord->notes,key,true,false,false);//not sure about trues here when only returning one...unnecessary?
        return prog;
    }
    
    
    /*
     Cm13 in of G returns IVm13 as the first option...use that by default
     */
    static std::vector<std::string> getFunctionFromChordString(std::string chordstring, NotePtr key = Note::create("C")){
        std::vector<std::string> prog = Progression::determine(chordstring,key,true,false,false);//not sure about trues here when only returning one...unnecessary?
        return prog;
    }
    
    static ChordPtr tupleToChord(ChordTuple tuple, NotePtr key){
        ChordPtr chord;
        NotePtr transKey = key;
        
        
        if(tuple.accidentals!=0){
            transKey = key->getTransposed(tuple.accidentals);
        }
        
        
        if(tuple.suffix == "7" || tuple.suffix == ""){
            tuple.roman += tuple.suffix;
            chord = Progression::getChordFromRoman(tuple.roman, transKey);
        }else{
            chord = Progression::getChordFromRoman(tuple.roman, transKey);
            if(chord)
            {
                chord = Chord::chordFromShorthand(tuple.suffix,chord->notes[0]);//add dim etc
            }
#ifdef LOGS
            else
            {
                ofLogWarning()<<"Progression:: tupleToChord failed to find chord "<<tuple.roman<<std::endl;
            }
#endif // LOGS
        }
        
        //Let the accidentals do their work
         int acc = tuple.accidentals;
        
        //this seems better for Ableton
        //chord->setOctave(3);
        //needed?
         //for(int i=0;i<chord.notes.size();i++){
         //  chord.notes[i].transpose(acc);
         //}
        
        return chord;
        
    }
    
    /*
     Returns a tuple (roman numeral, accidentals, chord suffix).
     {{{
     >>> progressions.parse_std::std::string("I")
     ('I', 0, '')
     >>> progressions.parse_std::std::string("bIM7")
     ('I', -1, 'M7')
     */
    
    
    static ChordTuple parse(std::string progression){
        boost::replace_all(progression, " ", "");
        std::string roman_numeral = "";
        
        ChordTuple tuple;
        
        
        //http://www.codeproject.com/Articles/9099/The-30-Minute-Regex-Tutorial
        
        boost::regex accEx{ "[#b]*(?=[iIvV])" };//find accidentals
        boost::smatch match;
        std::string acc;

        if (boost::regex_search(progression, match, accEx))
            {
            acc = match[0].str();
        }
        
        
        int augs = utils::getNumberOfSharps(acc);
        int dims = utils::getNumberOfFlats(acc);
        tuple.accidentals = utils::getNumberOfAccidentals(acc);

        std::string roman;
        roman = progression.substr((augs+dims));//remove accidentals
      
        boost::regex romanEx{ "(?<!d)[iIvV]*" };//find all romans not prefixed by d, thus exclude i in dim..
        if (boost::regex_search(roman, match, romanEx))
        {
            roman = match.str();
        }
        
        std::transform(roman.begin(), roman.end(), roman.begin(), ::toupper);
        tuple.roman = roman;

        std::string suffix = progression;
        tuple.suffix = progression.substr(roman.size()+augs+dims);//get suffix if any
        
        
        tuple.cleanedAccidentals = Progression::cleanAccidentals(tuple.accidentals);
        
        //fix for the subtonic/leadingtone mixup
        
        if((tuple.roman=="VII" && tuple.cleanedAccidentals=="b") || (tuple.roman=="VI" && tuple.cleanedAccidentals=="#") )
        {
            tuple.roman="bVII";
            tuple.cleanedAccidentals=="";
            tuple.accidentals=0;
        }
        
        return tuple;
   }
    

    
    static ChordPtr getChordFromRoman(std::string romanSymbol,NotePtr key){
        if(romanSymbol ==""){
            std::cout<<"Progression::getChordFromRoman empty symbol"<<std::endl;
            return 0;
        }
        
        ChordPtr cc = Chord::create();
        if(ChordFunctions[romanSymbol]){
            cc= ChordFunctions[romanSymbol](key);
            //std::cout<<"getChordFromRoman "<<romanSymbol<<" in "<<key->getName()<<" becomes "<<cc<<std::endl;
        }else{
            std::cout<<"Progression::getChordFromRoman error not found: "<<romanSymbol<<std::endl;
        }
        return cc;
    }
    
    /*
     Does simple harmonic substitutions. Returns a 
     list of possible substitions for `progression[substitute_index]`. 
     If `ignore_suffix` is set to True the suffix of the chord being substituted 
     will be ignored. Otherwise only progressions without a suffix, or with suffix '7' 
     will be substituted. The following table is used to convert progressions:
     || I || III ||
     || I || VI ||
     || IV || II ||
     || IV || VI ||
     || V || VII ||
     */
    
    
    static std::vector<std::string> substituteHarmonic(std::string chordFunction, bool ignore_suffix = false){

        std::vector<std::string> result;
    //turn into map to speed up std::map<std::string,std::string[]>
        std::string simple_substitutions[5][2] = {
            {"I", "III"},
            {"I", "VI"},
            {"IV", "II"},
            {"IV", "VI"},
            {"V", "VII"}};
                            
	       
        ChordTuple tuple = Progression::parse(chordFunction);
        
        std::string substituteRoman;
        Chord chord;
        
        if(tuple.suffix == "" || tuple.suffix == "7" || ignore_suffix){
            for (int s = 0;s<5;s++){
                substituteRoman="";
                
                if(tuple.roman==simple_substitutions[s][0]){
                    substituteRoman = simple_substitutions[s][1];//swap one for the other whichever side its found
                }else if(tuple.roman==simple_substitutions[s][1]){
                    substituteRoman = simple_substitutions[s][0];
                }
                
                if(substituteRoman!=""){
                    std::string chStr = tuple.cleanedAccidentals+substituteRoman+tuple.suffix;//rebuild chord
                    result.push_back(chStr);
                }
                
                
                
            }
        }
        
        return result;
    
    }
    
    
    /*
     Substitutes minor chords for its major equivalent. Recognizes 'm' and 'm7' suffixes \
     and ['II', 'III', 'VI'] if there is no suffix.
     {{{
     >>> progressions.substitute_minor_for_major(["VI"], 0)
     ["I"]
     >>> progressions.substitute_minor_for_major(["Vm"], 0)
     ["bVIIM"]
     >>> progressions.substitute_minor_for_major(["VIm7"], 0)
     ["IM7"]
     }}}
     */
    
    
    static std::vector<std::string> substituteMinorForMajor(std::string chordFunction, bool ignore_suffix = false){

    
        ChordTuple tuple = Progression::parse(chordFunction);
        std::vector<std::string> result;
    
	
        std::string newRoman,newAcc;
        int i,a;
        
        //Minor to major substitution
        if(tuple.suffix == "m" || tuple.suffix == "m7" || (tuple.suffix == "" && (tuple.roman == "II" ||tuple.roman ==  "III" || tuple.roman ==  "VI")) || ignore_suffix){
            
            i = RomanLookup[tuple.roman];
            newRoman = ROMAN[(i+2) % 7];
            a = Progression::intervalDiff(tuple.roman,newRoman, 3) + tuple.accidentals;
            
            if(tuple.suffix =="m" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman+"M");
            }
            
            if(tuple.suffix =="m7" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman+"M7");
            }
            
            if(tuple.suffix =="" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman);
            }

        }
        
        return result;

    }
    
    
    /*
     Substitutes major chords for their minor equivalent. Recognizes 'M' and 'M7' suffixes \
     and ['I', 'IV', 'V'] if there is no suffix.
     {{{
     >>> progressions.substitute_major_for_minor(["I"], 0)
     ["VI"]
     >>> progressions.substitute_major_for_minor(["VM7"], 0)
     ["IIIm7"]
     */
    
    
    static std::vector<std::string> substituteMajorForMinor(std::string chordFunction, bool ignore_suffix = false){
        ChordTuple tuple = Progression::parse(chordFunction);
        std::vector<std::string> result;

        std::string newRoman,newAcc;
        int i,a;

        
        //Major to minor substitution
        if(tuple.suffix == "M" || tuple.suffix == "M7" || (tuple.suffix == "" && (tuple.roman == "I" || tuple.roman == "IV"|| tuple.roman == "V")) || ignore_suffix){
            
            i = RomanLookup[tuple.roman];
            newRoman = ROMAN[(i+5) % 7];
            a = Progression::intervalDiff(tuple.roman,newRoman, 9) + tuple.accidentals;
            
            
            if(tuple.suffix =="M" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman+"m");
            }
            
            if(tuple.suffix =="M7" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman+"m7");
            }
            
            if(tuple.suffix =="" || ignore_suffix){
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman);
            }
            
        }
        return result;
        
    }
		  
    
    /*
     Substitutes a diminished chord for another diminished chord. Recognizes the 'dim' and 'dim7' \
     suffix and "VI" if there is no suffix.
     {{{
     >>> progressions.substitute_diminished_for_diminished(["VII"], 0)
     ["IIdim", "bIVdim", "bbVIdim"]
     */
    
    
    
    static std::vector<std::string>  substituteDiminishedForDiminished(std::string chordFunction, bool ignore_suffix = false){
        ChordTuple tuple = Progression::parse(chordFunction);
        std::vector<std::string> result;
        
        std::string newRoman,newAcc;
         
        int i,a = tuple.accidentals;
        
        //Diminished progressions
        if(tuple.suffix == "dim7" || tuple.suffix == "dim" || (tuple.suffix == "" && tuple.roman== "VII") || ignore_suffix){
  
            if(tuple.suffix == ""){
                tuple.suffix = "dim";
            }
            
            //Add diminished chord
            std::string last = tuple.roman;
            for(int x=0;x<3;x++){
                i = RomanLookup[last];
                newRoman = ROMAN[(i+2) % 7];
                a += Progression::intervalDiff(last, newRoman, 3);
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+newRoman+tuple.suffix);
                last = newRoman;
            }
        }
        return result;
    }
    
    
    
    
    
    
    static std::vector<std::string>  substituteDiminishedForDominant(std::string chordFunction, bool ignore_suffix = false){
        ChordTuple tuple = Progression::parse(chordFunction);
        std::vector<std::string> result;
        
        std::string newRoman,newAcc,domRoman;
        
        int i,a = tuple.accidentals;

        
        //Diminished progressions
        if(tuple.suffix == "dim7" || tuple.suffix == "dim" || (tuple.suffix == "" && tuple.roman == "VII") || ignore_suffix){
	
            if(tuple.suffix == ""){
                tuple.suffix = "dim";
            }
            //Add diminished chord
            std::string last = tuple.roman;
            for(int x=0;x<4;x++){
                i = RomanLookup[last];
                newRoman = ROMAN[(i+2) % 7];
                domRoman = ROMAN[(i+5) % 7];
                a = Progression::intervalDiff(last, domRoman, 8)+tuple.accidentals;
                newAcc = Progression::cleanAccidentals(a);
                result.push_back(newAcc+domRoman+"dom7");
                last = newRoman;
            }
        }
        return result;
    }
    
    
    /*
     Gives a list of possible substitutions for 
     `progression[substitute_index]`. If depth > 0 the substitutions 
     of each result will be recursively added as well.
     {{{
     >>> progressions.substitute(["I", "IV", "V", "I"], 0)
     ["III", "VI", etc.
     */
    
    static std::vector<std::string>  substitute(std::string chordFunction, int depth=0){
        
        ChordTuple tuple = Progression::parse(chordFunction);
        std::vector<std::string> result;
        
         std::string simple_substitutions[9][2] = {
             {"I", "III"},
             {"I", "VI"},
             {"IV", "II"},
             {"IV", "VI"},
             {"V", "VII"},
             {"V", "VIIdim7"},
             {"V", "IIdim7"},
             {"V", "IVdim7"},
             {"V", "bVIIdim7"}};
                                
         

         
        //Do the simple harmonic substitutions
        if(tuple.suffix == "" || tuple.suffix == "7"){
            
            
        }
        
        /*
         for subs in simple_substitutions:
         r = None
         if roman == subs[0]:
         r = subs[1]
         elif roman == subs[1]:
         r = subs[0]
         if r != None:
         res.append(tuple_to_std::std::string((r, acc, '')))
             //Add seventh or triad depending on r
         if r[-1] != "7":
         res.append(tuple_to_std::std::string((r, acc, '7')))
         else:
         res.append(tuple_to_std::std::string((r[:-1], acc, '')))
         
         
             //Add natural seventh
         if suff == '' or suff == 'M' or suff == 'm':
         res.append(tuple_to_std::std::string((roman, acc, suff + '7')))
         
         
             //Minor to major substitution
         if suff == 'm' or suff == 'm7':
         n = skip(roman, 2)
         a = interval_diff(roman, n, 3) + acc
         res.append(tuple_to_std::std::string((n, a, 'M')))
         res.append(tuple_to_std::std::string((n, a, 'M7')))
         
             //Major to minor substitution
         if suff == 'M' or suff == 'M7':
         n = skip(roman, 5)
         a = interval_diff(roman, n, 9) + acc
         res.append(tuple_to_std::std::string((n, a, 'm')))
         res.append(tuple_to_std::std::string((n, a, 'm7')))
         
         
             //Diminished progressions
         if suff == 'dim7' or suff == 'dim':
         
             //Add the corresponding dominant seventh
         res.append(tuple_to_std::std::string((skip(roman, 5), acc, 'dom7')))
         
         
             //Add chromatic dominant seventh
         n = skip(roman, 1)
         res.append(tuple_to_std::std::string((n, acc + interval_diff(roman, n, 1), 'dom7')))
         
             //Add diminished chord
         last = roman
         for x in range(4):
         next = skip(last, 2)
         acc += interval_diff(last, next, 3)
         res.append(tuple_to_std::std::string((next , acc, suff)))
         last = next
         
         res2 = []
         if depth > 0:
         for x in res:
         new_progr = progression
         new_progr[substitute_index] = x
         res2 += substitute(new_progr, substitute_index, depth - 1)
             return res + res2;
         */
    
        return result;
    }
    
    
    /*
     
     Determines the harmonic function of chord in key. This function can also deal with lists of chords.
     {{{
     >>> determine(["C", "E", "G"], "C")
     ['tonic']
     >>> determine(["G", "B", "D"], "C")
     ['dominant']
     >>> determine(["G", "B", "D", "F"], "C", True)
     ['V7']
     >>> determine([["C", "E", "G"], ["G", "B", "D"]], "C", True)
     [['I'], ['V']]

     */
    
    
    static std::vector< std::vector<std::string> > determine(std::vector< std::deque<NotePtr> > notes, NotePtr key, bool shorthand = false){
        std::vector< std::vector<std::string> > result;
        
        for(int i=0;i<notes.size();i++){
            result.push_back(Progression::determine(notes[i],key,shorthand));
        }
        
        return result;
        
    }
    
    static std::vector< std::vector<std::string> > determine(std::deque<ChordPtr> chords, NotePtr key, bool shorthand = false){
        std::vector< std::vector<std::string> > result;
        
        for(int i=0;i<chords.size();i++){
            result.push_back(Progression::determine(chords[i]->notes,key,shorthand));
        }
        
        return result;
        
    }
    
    
    static std::vector< std::vector<std::string> > determine(std::vector<ChordPtr> chords, NotePtr key, bool shorthand = false){
        std::vector< std::vector<std::string> > result;
        
        for(int i=0;i<chords.size();i++){
            result.push_back(Progression::determine(chords[i]->notes,key,shorthand));
        }
        
        return result;
        
    }
    /*
     <Cm7,Adim,E7b9>
     If using poly chords can return more complex relationships, hence nested vector
     */
    static std::vector< std::vector<std::string> > determine(std::vector<std::string> chordNames, NotePtr key, bool shorthand = false, bool useInversions = true, bool usePoly = true){
        std::vector< std::vector<std::string> > result;
        for(std::string chordName:chordNames){
            std::vector<std::string>progressionInterpretation;
            result.push_back(progressionInterpretation);
        }
        for(int i=0;i<chordNames.size();i++){
            std::string chordName = chordNames[i];
            //std::cout<<"Here chordName "<<chordName<<std::endl;
            ChordPtr chord = Chord::create(chordName);
            //Chord::print(chord->notes);
            if(chord->isValid()){
                std::vector<std::string>progressionInterpretation = Progression::determine(chord->notes,key,shorthand,useInversions,usePoly);
                for(int ii=0;ii<progressionInterpretation.size();ii++){
                    result[i].push_back(progressionInterpretation[ii]);
                }
                Progression::print(progressionInterpretation);
                
            }
#ifdef LOGS
            else
            {
                ofLogWarning()<<__FUNCTION__<<" "<<chordName<<" not valid"<<std::endl;
                
            }
#endif // LOGS
        }
        
        return result;
        
    }
    
    
    static std::vector<std::string> determine(std::string chordName, NotePtr key, bool shorthand = false, bool useInversions = true, bool usePoly = true){
        
        boost::replace_all(chordName," ","");
        
        //check for poly and slash chords
        std::vector<std::string> slash = utils::splitString(chordName, "/");//different bass
        std::vector<std::string> poly = utils::splitString(chordName, "|");//combined chords
        
        std::string func = "";
        if(poly.size()==2 && poly[0]!=poly[1]){
            std::string top = Progression::getFunctionInRoman(poly[0],key,shorthand);
            std::string bottom = Progression::getFunctionInRoman(poly[1],key,shorthand);
            func = top+"|"+bottom;
        }else if(slash.size()==2){
            std::string top = Progression::getFunctionInRoman(slash[0],key,shorthand);
            std::string bass = Progression::getFunctionInRoman(slash[1],key,shorthand);
            func = top+"/"+bass;
        }else{
            func = Progression::getFunctionInRoman(chordName,key,shorthand);
        }
        
        //this ensures that the first one is the one as it is written
        std::vector<std::string> result;
        result.push_back(func);
        
        if(useInversions || usePoly){
            ChordPtr chord = Chord::getChordFromString(chordName);
            if(chord->isValid()){
                std::vector<std::string> permutations = Progression::determine(chord->notes,key,shorthand,useInversions,usePoly);
                for(int i=0;i<permutations.size();i++){
                    if(permutations[i]!=func){
                        result.push_back(permutations[i]);
                    }
                }
            }
#ifdef LOGS
            else
            {
                ofLogWarning()<<__FUNCTION__<<" "<<chordName<<" not valid"<<std::endl;
            }
#endif // LOGS
        }

        return result;
    }
    
    
    static std::vector<std::string> determine(std::deque<NotePtr> notes, NotePtr key, bool shorthand = true, bool useInversions = true, bool usePoly = false){
        std::vector<std::string> result;
        if(notes.size()==0){
#ifdef LOGS
            ofLogWarning()<<"Warning: Progression::determine empty notes"<<std::endl;
            return result;
#endif // LOGS
        }
        
        
        //std::cout<<"Progression::determine"<<std::endl;
        //Chord::print(notes);
        
        std::vector<std::string> type_of_chord = Chord::determine(notes, true, useInversions, usePoly);//shorthand,inversion,poly
        
        if(!type_of_chord.size()){
            std::cout<<std::endl;
            Chord::print(notes);
#ifdef LOGS
            ofLogWarning()<<" don't give a type"<<std::endl;
#endif // LOGS
        }


        for(int i=0;i<type_of_chord.size();i++){
            std::string chordStr = type_of_chord[i];
           // std::cout<<"chordStr "<<chordStr<<std::endl;
            
            //check for poly and slash chords
            std::vector<std::string> slash = utils::splitString(chordStr, "/");//different bass
            std::vector<std::string> poly = utils::splitString(chordStr, "|");//combined chords
            
            
            
            std::string func = "";
            if(poly.size()==2 && poly[0]!=poly[1]){
                std::string top = Progression::getFunctionInRoman(poly[0],key,shorthand);
                std::string bottom = Progression::getFunctionInRoman(poly[1],key,shorthand);
                func = top+"|"+bottom;
            }else if(Chord::isSlashChord(chordStr)){
                std::string top = Progression::getFunctionInRoman(slash[0],key,shorthand);
                std::string bass = Progression::getFunctionInRoman(slash[1],key,shorthand);
                func = top+"/"+bass;
            }else{
                func = Progression::getFunctionInRoman(chordStr,key,shorthand);

            }
            
            if(func != "")
            {
                //ofLogVerbose()<<chordStr<<" is "<<func<<" in key "<<key->getName()<<std::endl;
                // Add to results
                result.push_back(func);
            }
#ifdef LOGS
            else
            {
                ofLogError() << chordStr << " cannot be parsed by Progression::determine" << std::endl;
            }
#endif // LOGS
        }
        
        return result;
    }
    
    /*
    TODO: Use Interval::toRoman and simplify
    */
                                                                            
    static std::string getFunctionInRoman(std::string chordStr,NotePtr key, bool shorthand = true){
        
        
        
        //this only affects shorthand false
        std::map<std::string, std::string> func_dict = {
        {"I","tonic"},
        {"II","supertonic"},
        {"III","mediant"},
        {"IV","subdominant"},
        {"V","dominant"},
        {"VI","submediant"},
        {"#VI","subtonic"},
        {"bVII","subtonic"},
        {"VII","leadingtone"}
        };
        


        
        ChordPtr chord = Chord::getChordFromString(chordStr);
        
        if(!chord)
        {
#ifdef LOGS
            ofLogError()<<"Chord "<<chordStr<<" not recognied by Progression::getFunctionInRoman"<<std::endl;
#endif // LOGS
            return "";
        }
        //std::cout<<"Determine function for "<<chordStr<<" "<<chord->name<<std::endl;
        NotePtr root = chord->getRoot();
        
        //std::string chord_type = chord.getChordSymbol();
        
        std::string chord_type = chord->name;
        
        //Determine chord function
        
        std::string intv = Interval::determine(key, root);
        std::vector<std::string> intvArr = utils::splitString(intv," ");
        
        //is this always size 2? These python std::string functions aren't very nice at all
        //write a Interval::getRoman
        std::string interval_type = intvArr[0];
        std::string interval = intvArr[1];
        std::string func="";
        
        if(interval == "unison"){
            func = "I";
        }else if(interval == "second"){
            func = "II";//was lower, but I don't think it makes sense since a song can be said to be in F lydian and then ii is not minor but G mixo. Also just harder to read since ii seems to indicate minor, but isn't necesarily, eg. Im7 iiM7
        }else if(interval == "third"){
            func = "III";//was lower
        }else if(interval == "fourth"){
            func = "IV";
        }else if(interval == "fifth"){
            func = "V";
        }else if(interval == "sixth"){
            func = "VI";//was lower
        }else if(interval == "seventh"){
            func = "VII";//was lower
        }
        
        
        if(shorthand){
            func += chord_type;
        }else{
            func = func_dict[func] + Chord::getFullName(chord_type);
        }
    
        /*
    
         
        std::vector<std::string> M = {"M","M7"};
        std::vector<std::string> m = {"m","m7"};
        std::vector<std::string> M7 = {"M","7"};
        std::vector<std::string> dim = {"dim","m7b5"};
        
        std::map<std::string, std::vector<std::string> > expected_chord = {
        {"I",M},
        {"IV",M},
        {"II",m},
        {"III",m},
        {"VI",m},
        {"VII",m},
        {"V",M7},
        {"VII",dim}
        };


        
        //Check whether the chord is altered or not
        typedef std::map<std::string, std::vector<std::string> >::iterator it_type;
        for(it_type iterator = expected_chord.begin(); iterator != expected_chord.end(); iterator++) {
            if(iterator->first == func){
                
                //Triads
                if(chord_type == iterator->second[0]){
                    if (!shorthand){
                        func = func_dict[func];
                    }
                    //Sevenths
                }else if(chord_type == iterator->second[1]){
                    if (shorthand){
                        func += "7";
                    }else{
                        func = func_dict[func] + " seventh";
                    }
                    //Other
                }else{
                    if(shorthand){
                        func += chord_type;
                    }else{
                        func = func_dict[func] + Chord::getFullName(chord_type);
                    }
                }
            }
        }
        
        */
        
        
        //Handle b's and #'s (for instance Dbm in key C is bII)
        if (shorthand){
            if (interval_type == "minor"){
                func = "b" + func;
            }else if( interval_type == "augmented"){
                func = "#" + func;
            }else if( interval_type == "diminished"){
                func = "bb" + func;
            }
        }else{
            if (interval_type == "minor"){
                func = "minor " + func;
            }else if( interval_type == "augmented"){
                func = "augmented " + func;
            }else if( interval_type == "diminished"){
                func = "diminished " + func;
            }
        }
        
        
        //std::cout<<"Interval: "<<interval_type<<" "<<interval<<" key: "<<key<<" root: "<<root<<" function: "<<func<<" symbol: " <<chord_type<<std::endl;
        
        return func;
    }
    
    /*
    Aliases
    */
    //Should work http://stackoverflow.com/questions/3053561/how-do-i-assign-an-alias-to-a-function-name-in-c
    //static std::vector<std::string> & analyse = determine;
    
    //const auto& analyse = determine;
    

     static std::vector<std::vector<std::string>> analyse(std::string chordNames, std::string keyString, bool shorthand = true, bool useInversions = true, bool usePoly = false){
        NotePtr key = Note::create(keyString);
        boost::replace_all(chordNames, " ", "");
        std::vector<std::string>splitChords = utils::splitString(chordNames, ",");
        return Progression::determine(splitChords, key, shorthand, useInversions, usePoly);
    }
    
    
    /*
    
    
    First bar in Giant Steps
    std::string progression = Progression::quickAnalysis("BM7,D7,GM7,Bb7,EbM7","G");
    gives IIIM7,V7,I7,bIII7,bVIM7
    
    or #VM7,VII7,IIIM7,V7,I7 if considered in Eb
    */
    
    static std::string quickAnalysis(std::string chordNames, std::string keyString){
        bool shorthand = true;
        bool useInversions = false;
        bool usePoly = false;
        
        NotePtr key = Note::create(keyString);

        boost::replace_all(chordNames," ","");

        std::vector<std::string>splitChords = utils::splitString(chordNames, ",");
        
        std::vector<std::vector<std::string>> progStrs = Progression::determine(splitChords, key, shorthand, useInversions, usePoly);
        
        //deflate the nested structure to first options of each interpretation
        std::vector<std::string>firstOpt;
        for(std::vector<std::string> intrpt:progStrs){
            if(intrpt.size()){
                firstOpt.push_back(intrpt.front());
            }else{
                firstOpt.push_back("?");
            }
        }

        return boost::algorithm::join(firstOpt, ",");
    }
    
    /*
     Convenience
     */
    
    static void print(std::deque<Chord> chords){
        
        std::cout <<"[ ";
        for(int i = 0;i<chords.size();i++){
            std::cout<<chords[i];
            if(i<chords.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<<std::endl;
    }
    
    static void print(std::vector<std::string> substitutes){
        
        std::cout <<"[ ";
        for(int i = 0;i<substitutes.size();i++){
            std::cout<<substitutes[i];
            if(i<substitutes.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<<std::endl;
    }
    
    static void print(std::vector< std::vector<std::string> > prog){
        
        std::cout <<"Progression [ ";
        for(int i = 0;i<prog.size();i++){
            std::cout <<"[";
            for(int ii = 0;ii<prog[i].size();ii++){
                
                std::cout<<prog[i][ii];
                if(ii<prog[i].size()-1){
                    std::cout<<", ";
                }
            }
            std::cout <<"]";
            if(i<prog.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<<std::endl;
    }
    
    
    static void print(std::deque<ChordPtr> chords){
        
        std::cout <<"[ ";
        for(int i = 0;i<chords.size();i++){
            std::cout<<chords[i];
            if(i<chords.size()-1){
                std::cout<<", ";
            }
        }
        std::cout<<" ]"<<std::endl;
    }
    
private:
    
    /*
     It just wacks the accidentals back unto the std::string as normal b or # symbols
     */
    static std::string cleanAccidentals(int accidentals){
        std::string acc="";
        
        if( accidentals > 6){
            accidentals = - accidentals % 6;
        }else if(accidentals < -6){
            accidentals = accidentals % 6;
        }
        
            
        if(accidentals>0){
            std::string aug = "##############################################";
            acc = aug.substr(0,accidentals);//this must be quicker than a for loop?
        }
        
        if(accidentals<0){
            std::string dims = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            acc = dims.substr(0,abs(accidentals));//this must be quicker than a for loop?
        }
        
        return acc;
    }
	
    /*
     Returns the number of half steps progression2 needs to be \
     diminished or augmented until the interval between `progression1` \
     and `progression2` is `interval`
     */
    static int intervalDiff(std::string progression1, std::string progression2,int interval){

   
        int i = numeral_intervals[RomanLookup[progression1]];
        int j = numeral_intervals[RomanLookup[progression2]];
        
        int acc = 0;
        if(j < i){
            j += 12;
        }
        while((j - i) > interval){
            acc--;
            j--;
        }
        while((j - i) < interval){
            acc++;
            j++;
        }
        return acc;
     }
    
};//class


//typedef shared_ptr<Progression> ProgressionPtr;


};//namespace

#endif
