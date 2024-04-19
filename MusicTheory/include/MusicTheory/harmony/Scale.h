/*
 *  Scales.h
 *  MusicTheory
 *
 *  Created by Andreas Borg on 02/02/2013
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *

 ================================================================================

 The scales module allows you to create a plethora of scales. Here's a
 little overview:

 === The diatonic scale and its modes ===
 * diatonic
 * ionian
 * dorian
 * phrygian
 * lydian
 * mixolydian
 * aeolian
 * locrian

 === The minor scales ===
 * natural_minor
 * harmonic_minor
 * melodic_minor

 === Other scales ===
 * chromatic
 * whole_note
 * diminished


 ================================================================================

 */


#ifndef _Scale
#define _Scale

#include <filesystem>
#include <fstream>

#include "Interval.h"
#include "Note.h"
#include "Diatonic.h"

namespace MusicTheory {
	class Scale;
	typedef std::shared_ptr<Scale>(*ScaleFunctionPointer)(NotePtr);
	typedef std::map<std::string, ScaleFunctionPointer> ScaleFunctionLookup;

	//===================================================================
#pragma mark - CHORD SCALES
//===================================================================

/*
The obvious scales are first in list.
More creative alternatives later.
You can load your own using Scale::loadChordScales(std::string fileName);
where each line should be formatted
chordSymbol tab comma separated list, eg.

m   dorian,aeolian
M   ionian
Any chord found in this list will replace these options for only that chord
*/

	static Lookup ChordScaleLookup = {
		//Triads
		{"m","dorian,aeolian"},
		{"M","ionian"},
		{"","ionian"},
		{"dim","diminished"},

		//Augmented Chord
		{"aug","augmented,pentatonicMinorbIII"},
		{"+","augmented,pentatonicMinorbIII"},


		//M
		{"M7+5","melodicMinorIII,augmented,hindu"},//lydian aug
		{"M7+","melodicMinorIII,augmented,hindu"},

		//m
		{"m7+","aeolian,dorian,phrygian,pentatonicMinorbIII"},
		{"m7+5","aeolian,dorian,phrygian,pentatonicMinorbIII"},

		//Dom
		{"7+","melodicMinorVII,melodicMinorV,pentatonicMinorbIII,wholenote"},//mixolydian b6
		{"7+5","melodicMinorVII,melodicMinorV,pentatonicMinorbIII,wholenote"},
		{"7#5","melodicMinorVII,melodicMinorV,pentatonicMinorbIII,wholenote"},

		//Suspended Chord
		{"sus47","pentatonicMinorV,dorian,aeolian,phrygian,mixolydian"},//tricky as no min/maj third
		{"sus4","pentatonicMinorV,harmonicMinor,melodicMinor,pentatonicMinor,blues,aeolian,dorian,phrygian,mixolydian"},
		{"sus2","harmonicMinor,melodicMinor,pentatonicMinor,blues"},
		{"sus","ionian,harmonicMinor,melodicMinor,pentatonicMinor,blues,aeolian,dorian,phrygian,mixolydian"},
		{"11","mixolydian"},
		{"sus4b9","melodicMinorII"},
		{"susb9","melodicMinorII"},
		{"sus9","pentatonicMinorV,dorian,aeolian,phrygian,mixolydian"},
		{"sus49","pentatonicMinorV,dorian,aeolian,phrygian,mixolydian"},





		//Sevenths
		{"m7","dorian,aeolian,pentatonicMinor,pentatonicMinorII,pentatonicMinorV"},
		{"M7","ionian,lydian,pentatonicMinorVII,pentatonicDominantII"},
		{"dom7","mixolydian,pentatonicMajor,pentatonicDominant,pentatonicDominantII,pentatonicMinorV"},
		{"7","mixolydian,pentatonicMajor,pentatonicDominant,pentatonicDominantII,pentatonicMinorV"},
		{"m7b5","locrian,melodicMinorVI,pentatonicDominantbVI"},
		{"dim7","diminished"},
		{"dim","diminished"},
		{"mM7","melodicMinor,harmonicMinor"},



		//Sixths
		{"m6","melodicMinor"},
		{"M6","ionian,lydian"},
		{"6","ionian,lydian"},
		{"6/7",""},
		{"67", ""},
		{"6/9","pentatonicMajor"},
		{"69","pentatonicMajor"},

		//Ninths
		{"9","mixolydian,bebopDominant"},
		{"7b9","flamenco"},//so how to m7b9?
		{"7#9","halfDiminished"},
		{"M9","lydian"},
		{"m9","aeolian,dorian"},
		{"9#11","lydian"},
		{"m6/9","dorian"},
		{"m6/9/11","dorian"},

		//Elevenths
		{"7#11","melodicMinorIV"},
		{"m11","dorian,pentatonicMinorV,pentatonicMinorIV,pentatonicMinorIII,pentatonicMinor,blues"},
		{"M7#11","lydian"},

		//Thirteenths
		{"M13",""},
		{"m13","melodicMinor,bebopMinor"},

		{"13","mixolydian,melodicMinorIV,bebopDominant,pentatonicMajor,blues"},

		//Altered Chord b9 #9 #11 b13
		{"7b5","melodicMinorIV"},//lydian dominant
		{"7+#9","melodicMinorVII"},


		{"7+b9","melodicMinorVII"},


		//Special
		{"hendrix","blues,pentatonicMinorbIII"},
		{"7b12","blues"},

		{"5","pentatonicMinor,blues,aeolian,pentatonicMajor,pentatonicMinorIII,pentatonicMinorIV,pentatonicMinorV"},//as you please, since it's common to most scales


		{"7b9b5","melodicMinorVII,pentatonicMinorbIII"},//superlocrian
		{"m7b9","phrygian"},

		//altered, melodic minor
		{"m11b5","melodicMinorVI"},
		{"7#9b13","melodicMinorVII,pentatonicMinorbIII"}



	};

	//
	//
	//
	class Scale : public std::enable_shared_from_this<Scale> {

	public:

		Scale() {};

		std::deque<NotePtr> notes;
		std::string name;



		//===================================================================
#pragma mark - FACTORY METHODS
//===================================================================


		static std::shared_ptr<Scale>create() {
			return std::shared_ptr<Scale>(new Scale());
		}

		/*
		Argument needs to be formatted as getName() returns it,
		eg. A dorian
		*/
		static std::shared_ptr<Scale>create(std::string fullName) {
			std::shared_ptr<Scale> s = std::shared_ptr<Scale>(new Scale());

			auto parts = utils::splitString(fullName, " ");
			if (parts.size() == 2) {
				NotePtr root = Note::create(parts[0]);
				std::string name = parts[1];
				s = Scale::getScaleFromString(name, root);

			}
			return s;
		}

		std::shared_ptr<Scale> copy() {
			std::shared_ptr<Scale>s = std::shared_ptr<Scale>(new Scale(*this));
			s->notes.clear();
			for (NotePtr n : notes) {
				s->notes.push_back(n->copy());
			}
			return s;
		}

		//===================================================================
#pragma mark - INSTANCE METHODS
//===================================================================



		int size() {
			return notes.size();
		}

		std::string getName() {
			std::string fullName = "";

			if (notes.size()) {
				fullName += notes[0]->getDiatonicName();
			}

			fullName += " " + name;

			return fullName;

		}


		//===================================================================
#pragma mark - STATIC METHODS
//===================================================================

		static bool isValid(std::shared_ptr<Scale>s) {
			if (!s) {
				return false;
			}
			else {
				return s->size() > 1;
			}
		}

		static bool loadChordScales(std::string fileName) {
			if (!std::filesystem::exists(fileName)) {
#ifdef LOGS
				ofLogError() << "Missing chord scale file: " << fileName << std::endl;
#endif // LOGS
				return false;
			}

			std::ifstream infile(fileName);
			for (std::string line; getline(infile, line); )
			{
				//std::cout << line << std::endl;
				auto p = utils::splitString(line, "\t");

				if (p.size() > 1) {
					if (Chord::isValidName(p[0])) {

						boost::replace_all(p[1], " ", "");
						ChordScaleLookup[p[0]] = p[1];
#ifdef LOGS
						ofLogNotice() << "Setting chord scale for \"" << p[0] << "\" to " << p[1] << std::endl;
#endif // LOGS
					}
				}
			}

			return true;

		}



		static std::vector<std::string> getAllKnownScales() {
			static std::vector<std::string>_knownScales = {
				"ionian",
				"dorian",
				"phrygian",
				"lydian",
				"mixolydian",
				"aeolian",
				"locrian",
				"halfDiminished",
				"pentatonicMinor",
				"pentatonicMinorbII",
				"pentatonicMinorII",
				"pentatonicMinorbIII",
				"pentatonicMinorIII",
				"pentatonicMinorIV",
				"pentatonicMinorbV",
				"pentatonicMinorV",
				"pentatonicMinorbVI",
				"pentatonicMinorVI",
				"pentatonicMinorbVII",
				"pentatonicMinorVII",
				"pentatonicMajor",
				"pentatonicDominant",
				"pentatonicDominantbII",
				"pentatonicDominantII",
				"pentatonicDominantbIII",
				"pentatonicDominantIII",
				"pentatonicDominantIV",
				"pentatonicDominantbV",
				"pentatonicDominantV",
				"pentatonicDominantbVI",
				"pentatonicDominantVI",
				"pentatonicDominantbVII",
				"pentatonicDominantVII",
				"melodicMinor",
				"melodicMinorII",
				"melodicMinorIII",
				"augmented",
				"melodicMinorII",
				"melodicMinorIII",
				"melodicMinorIV",
				"melodicMinorV",
				"melodicMinorVI",
				"melodicMinorVII",
				"naturalMinor",
				"harmonicMinor",
				"flamenco",
				"≈",
				"bebopDominant",
				"bebopMinor",
				"blues",
				"lydianDiminished",
				"lydianDominant",
				"inSen",
				"hirajoshi",
				"hindu",
				"chromatic",
				"wholenote" };
			return _knownScales;
		}



		static std::vector<std::string> getAllDiatonicScales() {
			static std::vector<std::string>_diaScales = {
				"ionian",
				"dorian",
				"phrygian",
				"lydian",
				"mixolydian",
				"aeolian",
				"locrian" };
			return _diaScales;
		}


		static std::vector<std::string> getAllPentatonicScales() {
			static std::vector<std::string>_pentatonicScales = {
				"pentatonicMinor",
				"pentatonicMinorbII",
				"pentatonicMinorII",
				"pentatonicMinorbIII",
				"pentatonicMinorIII",
				"pentatonicMinorIV",
				"pentatonicMinorbV",
				"pentatonicMinorV",
				"pentatonicMinorbVI",
				"pentatonicMinorVI",
				"pentatonicMinorbVII",
				"pentatonicMinorVII",
				"pentatonicMajor",
				"pentatonicDominant",
				"pentatonicDominantbII",
				"pentatonicDominantII",
				"pentatonicDominantbIII",
				"pentatonicDominantIII",
				"pentatonicDominantIV",
				"pentatonicDominantbV",
				"pentatonicDominantV",
				"pentatonicDominantbVI",
				"pentatonicDominantVI",
				"pentatonicDominantbVII",
				"pentatonicDominantVII" };
			return _pentatonicScales;
		}


		static std::vector<std::string> getAllMelodicMinorScales() {
			static std::vector<std::string>_melMinScales = {
				"melodicMinor",
				"melodicMinorII",
				"melodicMinorIII",
				"augmented",
				"melodicMinorII",
				"melodicMinorIII",
				"melodicMinorIV",
				"melodicMinorV",
				"melodicMinorVI",
				"melodicMinorVII",
				"lydianDiminished",
				"lydianDominant" };
			return _melMinScales;
		}


		static std::vector<std::string> getAllDiminishedScales() {
			static std::vector<std::string>_dimScales = {
				"halfDiminished",
				"diminished",
				"lydianDiminished",
			};
			return _dimScales;
		}

		static std::vector<std::string> getAllEthnicScales() {
			static std::vector<std::string>_ethnicScales = {
				"flamenco",
				"inSen",
				"hirajoshi",
				"hindu" };
			return _ethnicScales;
		}



		//===================================================================
#pragma mark - Diatonic scales
//===================================================================



	//The diatonic scales and its modes
	/*
	 Returns the diatonic scale starting on note.
	 Example:
	 {{{
	 >>> diatonic("C")
	 ["C", "D", "E", "F", "G", "A", "B"]
	 */
		static std::deque<NotePtr> diatonic(NotePtr note) {
			std::deque<NotePtr>nts = Diatonic::getNotes(note->copy());
			Scale::setOctave(nts, note->getAbsoluteOctave());
			return nts;
		}
		static std::shared_ptr<Scale> getDiatonic(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "diatonic";//same as ionian, doremi etc
			scale->notes = Scale::diatonic(note->copy());
			return scale;
		}

		/*
		 Returns the ionian mode scale starting on note.
		 Example:
		 {{{
		 >>> ionian("C")
		 ["C", "D", "E", "F", "G", "A", "B"]
		 */

		static std::deque<NotePtr> ionian(NotePtr note) {
			return Scale::diatonic(note->copy());
		}

		static std::shared_ptr<Scale> getIonian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "ionian";//same as ionian, doremi etc
			scale->notes = Scale::diatonic(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getIonian(std::string note) {
			return Scale::getIonian(Note::create(note));
		}
		/*
		 Returns the dorian mode scale starting on note.
		 Example:
		 {{{
		 >>> dorian("D")
		 ["D", "E", "F", "G", "A", "B", "C"]

		 */

		static std::deque<NotePtr> dorian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::minorSeventh(note->copy()));
			ionian = Scale::offset(ionian, 1);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			//all other diatonic scale are created from the ionian
			return ionian;
		}

		static std::shared_ptr<Scale> getDorian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "dorian";
			scale->notes = Scale::dorian(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getDorian(std::string note) {
			return Scale::getDorian(Note::create(note));
		}

		/*
		 Returns the phrygian mode scale starting on note.
		 Example:
		 {{{
		 >>> phrygian("E")
		 ["E", "F", "G", "A", "B", "C", "D"]
		 */
		static std::deque<NotePtr> phrygian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::minorSixth(note->copy()));
			//all other diatonic scale are created from the ionian
			ionian = Scale::offset(ionian, 2);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			return ionian;
		}

		static std::shared_ptr<Scale> getPhrygian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "phrygian";
			scale->notes = Scale::phrygian(note->copy());
			return scale;
		}


		static std::shared_ptr<Scale> getPhrygian(std::string note) {
			return Scale::getPhrygian(Note::create(note));
		}

		/*
		 Returns the lydian mode scale starting on note.
		 Example:
		 {{{
		 >>> lydian("F")
		 ["F", "G", "A", B", "C", "D", "E"]
		 */

		static std::deque<NotePtr> lydian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::perfectFifth(note->copy()));
			ionian = offset(ionian, 3);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			return ionian;
		}

		static std::shared_ptr<Scale> getLydian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "lydian";
			scale->notes = Scale::lydian(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getLydian(std::string note) {
			return Scale::getLydian(Note::create(note));
		}

		/*
		 Returns the mixolydian mode scale starting on note.
		 Example:
		 {{{
		 >>> mixolydian("G")
		 ["G", "A", "B", "C", "D", "E", "F"]
		 */
		static std::deque<NotePtr> mixolydian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::perfectFourth(note->copy()));
			//all other diatonic scale are created from the ionian
			ionian = offset(ionian, 4);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			return ionian;
		}


		static std::shared_ptr<Scale> getMixolydian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "mixolydian";
			scale->notes = Scale::mixolydian(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMixolydian(std::string note) {
			return Scale::getMixolydian(Note::create(note));
		}


		/*
		 Returns the aeolian mode scale starting on note.
		 Example:
		 {{{
		 >>> aeolian("A")
		 ["A", "B", "C", "D", "E", "F", "G"]


		 also descending melodic minor
		 */

		static std::deque<NotePtr> aeolian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::minorThird(note->copy()));
			//all other diatonic scale are created from the ionian
			ionian = offset(ionian, 5);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			return ionian;
		}

		static std::shared_ptr<Scale> getAeolian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "aeolian";
			scale->notes = Scale::aeolian(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getAeolian(std::string note) {
			return Scale::getAeolian(Note::create(note));
		}

		/*
		 Returns the locrian mode scale starting on note.
		 Example:
		 {{{
		 >>> locrian("B")
		 ["B", "C", "D", "E", "F", "G", "A"]
		 */
		static std::deque<NotePtr> locrian(NotePtr note) {
			std::deque<NotePtr> ionian = Scale::ionian(Interval::minorSecond(note->copy()));
			//all other diatonic scale are created from the ionian
			ionian = offset(ionian, 6);
			Scale::setOctave(ionian, note->getAbsoluteOctave());
			return ionian;
		}

		static std::shared_ptr<Scale> getLocrian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "locrian";
			scale->notes = Scale::locrian(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getLocrian(std::string note) {
			return Scale::getLocrian(Note::create(note));
		}



		//The minor modes

		/*
		 Returns the natural minor scale starting on note.
		 Example:
		 {{{
		 >>> natural_minor("A")
		 ["A", "B", "C", "D", "E", "F", "G"]
		 */
		static std::deque<NotePtr> naturalMinor(NotePtr note) {
			std::deque<NotePtr> scale = Diatonic::getNotes(Interval::minorThird(note->copy()));
			scale = Scale::offset(scale, 5);
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getNaturalMinor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "naturalMinor";
			scale->notes = Scale::naturalMinor(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getNaturalMinor(std::string note) {
			return Scale::getNaturalMinor(Note::create(note));
		}


		//===================================================================
#pragma mark - Harmonic minor scales
//===================================================================



	/*
	 Returns the harmonic minor scale starting on note.
	 Example:
	 {{{
	 >>> harmonic_minor("A")
	 "A", "B", "C", "D", "E", "F", "G#"]
	 */
		static std::deque<NotePtr> harmonicMinor(NotePtr note) {
			std::deque<NotePtr> scale = Scale::naturalMinor(note->copy());
			scale[6]->augment();
			return scale;
		}

		static std::shared_ptr<Scale> getHarmonicMinor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "harmonicMinor";
			scale->notes = Scale::harmonicMinor(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getHarmonicMinor(std::string note) {
			return Scale::getHarmonicMinor(Note::create(note));
		}


		//===================================================================
#pragma mark - Melodic minor scales
//===================================================================



	/*
	 Returns the melodic minor scale starting on note.
	 Example:
	 {{{
	 >>> melodic_minor("A")
	 ["A", "B", "C", "D", "E", "F#", "G#"]

	 ascending melodic minor


	 */
		static std::deque<NotePtr> melodicMinor(NotePtr note) {
			std::deque<NotePtr> scale = Scale::harmonicMinor(note->copy());
			scale[5]->augment();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinor";
			scale->notes = Scale::melodicMinor(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinor(std::string note) {
			return Scale::getMelodicMinor(Note::create(note));
		}

		/*
		 Phrygian #6
		 Dorian b2
		 */
		static std::deque<NotePtr> melodicMinorII(NotePtr note) {
			std::deque<NotePtr> scale = Scale::phrygian(note->copy());
			scale[5]->augment();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorII";
			scale->notes = Scale::melodicMinorII(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorII(std::string note) {
			return Scale::getMelodicMinorII(Note::create(note));
		}

		static std::shared_ptr<Scale> getPhrygianRaisedSixth(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "phrygian#6";
			scale->notes = Scale::melodicMinorII(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getPhrygianRaisedSixth(std::string note) {
			return Scale::getPhrygianRaisedSixth(Note::create(note));
		}
		/*
		 Lydian augmented
		 Third of melodic minors
		 https://en.wikipedia.org/wiki/Lydian_augmented_scale
		 */

		static std::deque<NotePtr> melodicMinorIII(NotePtr note) {
			std::deque<NotePtr> scale = Scale::lydian(note->copy());
			scale[4]->augment();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorIII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorIII";
			scale->notes = Scale::melodicMinorIII(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorIII(std::string note) {
			return Scale::getMelodicMinorIII(Note::create(note));
		}
		static std::shared_ptr<Scale> getLydianAugmented(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "lydianAugmented";
			scale->notes = Scale::melodicMinorIII(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getLydianAugmented(std::string note) {
			return Scale::getLydianAugmented(Note::create(note));
		}
		/*
		 Lydian dominant
		 Mixolydian #4
		 */

		static std::deque<NotePtr> melodicMinorIV(NotePtr note) {
			std::deque<NotePtr> scale = Scale::lydian(note->copy());
			scale[6]->diminish();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorIV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorIV";
			scale->notes = Scale::melodicMinorIV(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorIV(std::string note) {
			return Scale::getMelodicMinorIV(Note::create(note));
		}
		static std::shared_ptr<Scale> getLydianDominant(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "lydianDominant";
			scale->notes = Scale::melodicMinorIV(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getLydianDominant(std::string note) {
			return Scale::getLydianDominant(Note::create(note));
		}


		/*
		 Mixolydian b6
		 */

		static std::deque<NotePtr> melodicMinorV(NotePtr note) {
			std::deque<NotePtr> scale = Scale::mixolydian(note->copy());
			scale[5]->diminish();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorV";
			scale->notes = Scale::melodicMinorV(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorV(std::string note) {
			return Scale::getMelodicMinorV(Note::create(note));
		}

		static std::shared_ptr<Scale> getMixolydianLoweredSixth(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "mixolydianLoweredSixth";
			scale->notes = Scale::melodicMinorV(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMixolydianLoweredSixth(std::string note) {
			return Scale::getMixolydianLoweredSixth(Note::create(note));
		}
		/*
		 Locrian #2
		 */

		static std::deque<NotePtr> melodicMinorVI(NotePtr note) {
			std::cout << __FUNCTION__ << " " << note << std::endl;
			std::deque<NotePtr> scale = Scale::locrian(note->copy());
			scale[1]->augment();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorVI(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorVI";
			scale->notes = Scale::melodicMinorVI(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorVI(std::string note) {
			return Scale::getMelodicMinorVI(Note::create(note));
		}

		static std::shared_ptr<Scale> getHalfDiminished(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "halfDiminished";
			scale->notes = Scale::melodicMinorVI(note->copy());
			return scale;
		}


		static std::shared_ptr<Scale> getHalfDiminished(std::string note) {
			return Scale::getHalfDiminished(Note::create(note));
		}

		/*
		 SuperLocrian
		 Altered dominant
		 Altered scale
		 */

		static std::deque<NotePtr> melodicMinorVII(NotePtr note) {
			std::deque<NotePtr> scale = Scale::locrian(note->copy());
			scale[3]->diminish();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorVII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "melodicMinorVII";
			scale->notes = Scale::melodicMinorVII(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getMelodicMinorVII(std::string note) {
			return Scale::getMelodicMinorVII(Note::create(note));
		}


		static std::shared_ptr<Scale> getSuperLocrian(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "superLocrian";
			scale->notes = Scale::melodicMinorVII(note->copy());
			return scale;
		}


		static std::shared_ptr<Scale> getSuperLocrian(std::string note) {
			return Scale::getSuperLocrian(Note::create(note));
		}




		//Other scales


		static std::deque<NotePtr> lydianDiminished(NotePtr note) {
			std::deque<NotePtr> scale = Scale::ionian(note->copy());
			scale[2]->diminish();
			scale[3]->augment();
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getLydianDiminished(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "lydianDiminished";
			scale->notes = Scale::lydianDiminished(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getLydianDiminished(std::string note) {
			return Scale::getLydianDiminished(Note::create(note));
		}



		//===================================================================
#pragma mark - Pentatonic scales
//===================================================================



		static std::deque<NotePtr> pentatonicMinor(NotePtr note) {
			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			scale.push_back(Interval::minorThird(note->copy()));
			scale.push_back(Interval::perfectFourth(note->copy()));
			scale.push_back(Interval::perfectFifth(note->copy()));
			scale.push_back(Interval::minorSeventh(note->copy()));
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinor";
			scale->notes = Scale::pentatonicMinor(note);
			return scale;
		}


		static std::shared_ptr<Scale> getPentatonicMinor(std::string note) {
			return Scale::getPentatonicMinor(Note::create(note));
		}


		static std::deque<NotePtr> pentatonicMajor(NotePtr note) {
			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			scale.push_back(Interval::majorSecond(note->copy()));
			scale.push_back(Interval::majorThird(note->copy()));
			scale.push_back(Interval::perfectFifth(note->copy()));
			scale.push_back(Interval::majorSixth(note->copy()));
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMajor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMajor";
			scale->notes = Scale::pentatonicMajor(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMajor(std::string note) {
			return Scale::getPentatonicMajor(Note::create(note));
		}

		static std::deque<NotePtr> pentatonicDominant(NotePtr note) {
			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			scale.push_back(Interval::majorSecond(note->copy()));
			scale.push_back(Interval::majorThird(note->copy()));
			scale.push_back(Interval::perfectFifth(note->copy()));
			scale.push_back(Interval::minorSeventh(note->copy()));
			Scale::setOctave(scale, note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominant(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominant";
			scale->notes = Scale::pentatonicDominant(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominant(std::string note) {
			return Scale::getPentatonicDominant(Note::create(note));
		}


		//===================================================================
#pragma mark - Pentatonic scales on different roots
//===================================================================


	/*
	Pentatonic minor starting on minor second away from note.
	Major pentatonic is the same starting from second (minor third) in
	the minor.

	Scofield & Mccoy Tyner concept
	https://www.youtube.com/watch?v=8ZMCbf1TfRc
	*/

		static std::deque<NotePtr> pentatonicMinorbII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(1));
		}

		static std::shared_ptr<Scale> getPentatonicMinorbII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorbII";
			scale->notes = Scale::pentatonicMinorbII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorbII(std::string note) {
			return Scale::getPentatonicMinorbII(Note::create(note));
		}


		/*
		Pentatonic minor starting on major second away from note
		*/

		static std::deque<NotePtr> pentatonicMinorII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(2));
		}

		static std::shared_ptr<Scale> getPentatonicMinorII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorII";
			scale->notes = Scale::pentatonicMinorII(note);
			scale->setOctave(note->getAbsoluteOctave());
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorII(std::string note) {
			return Scale::getPentatonicMinorII(Note::create(note));
		}

		/*
		Pentatonic minor starting on minor third away from note
		*/

		static std::deque<NotePtr> pentatonicMinorbIII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(3));
		}

		static std::shared_ptr<Scale> getPentatonicMinorbIII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorbIII";
			scale->notes = Scale::pentatonicMinorbIII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorbIII(std::string note) {
			return Scale::getPentatonicMinorbIII(Note::create(note));
		}

		/*
		Pentatonic minor starting on major third away from note
		*/

		static std::deque<NotePtr> pentatonicMinorIII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(4));
		}

		static std::shared_ptr<Scale> getPentatonicMinorIII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorIII";
			scale->notes = Scale::pentatonicMinorIII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorIII(std::string note) {
			return Scale::getPentatonicMinorIII(Note::create(note));
		}

		/*
		Pentatonic minor starting on fourth away from note
		*/

		static std::deque<NotePtr> pentatonicMinorIV(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(5));
		}

		static std::shared_ptr<Scale> getPentatonicMinorIV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorIV";
			scale->notes = Scale::pentatonicMinorIV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorIV(std::string note) {
			return Scale::getPentatonicMinorIV(Note::create(note));
		}

		/*
		Pentatonic minor starting on flat fifth away from note
		*/

		static std::deque<NotePtr> pentatonicMinorbV(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(6));
		}

		static std::shared_ptr<Scale> getPentatonicMinorbV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorbV";
			scale->notes = Scale::pentatonicMinorbV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorbV(std::string note) {
			return Scale::getPentatonicMinorbV(Note::create(note));
		}


		/*
		Pentatonic minor starting on perfect fifth away from note
		*/

		static std::deque<NotePtr> pentatonicMinorV(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(7));
		}

		static std::shared_ptr<Scale> getPentatonicMinorV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorV";
			scale->notes = Scale::pentatonicMinorV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorV(std::string note) {
			return Scale::getPentatonicMinorV(Note::create(note));
		}


		/*
		Pentatonic minor starting on minor sixth away from note
		*/

		static std::deque<NotePtr> pentatonicMinorbVI(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(8));
		}

		static std::shared_ptr<Scale> getPentatonicMinorbVI(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorbVI";
			scale->notes = Scale::pentatonicMinorbVI(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorbVI(std::string note) {
			return Scale::getPentatonicMinorbVI(Note::create(note));
		}



		/*
		Pentatonic minor starting on major sixth away from note
		*/

		static std::deque<NotePtr> pentatonicMinorVI(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(9));
		}

		static std::shared_ptr<Scale> getPentatonicMinorVI(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorVI";
			scale->notes = Scale::pentatonicMinorVI(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorVI(std::string note) {
			return Scale::getPentatonicMinorVI(Note::create(note));
		}


		/*
		Pentatonic minor starting on minor seventh away from note
		*/

		static std::deque<NotePtr> pentatonicMinorbVII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(10));
		}

		static std::shared_ptr<Scale> getPentatonicMinorbVII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorbVII";
			scale->notes = Scale::pentatonicMinorbVII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicMinorbVII(std::string note) {
			return Scale::getPentatonicMinorbVII(Note::create(note));
		}


		/*
		Pentatonic minor starting on major seventh away from note
		*/

		static std::deque<NotePtr> pentatonicMinorVII(NotePtr note) {
			return Scale::pentatonicMinor(note->getTransposed(11));
		}

		static std::shared_ptr<Scale> getPentatonicMinorVII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicMinorVII";
			scale->notes = Scale::pentatonicMinorVII(note);
			return scale;
		}



		static std::shared_ptr<Scale> getPentatonicMinorVII(std::string note) {
			return Scale::getPentatonicMinorVII(Note::create(note));
		}


		/*
		Dominant pentatonics with different roots
		*/

		static std::deque<NotePtr> pentatonicDominantbII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(1));
		}

		static std::shared_ptr<Scale> getPentatonicDominantbII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantbII";
			scale->notes = Scale::pentatonicDominantbII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantbII(std::string note) {
			return Scale::getPentatonicDominantbII(Note::create(note));
		}


		static std::deque<NotePtr> pentatonicDominantII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(2));
		}

		static std::shared_ptr<Scale> getPentatonicDominantII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantII";
			scale->notes = Scale::pentatonicDominantII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantII(std::string note) {
			return Scale::getPentatonicDominantII(Note::create(note));
		}




		static std::deque<NotePtr> pentatonicDominantbIII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(3));
		}

		static std::shared_ptr<Scale> getPentatonicDominantbIII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantbIII";
			scale->notes = Scale::pentatonicDominantbIII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantbIII(std::string note) {
			return Scale::getPentatonicDominantbIII(Note::create(note));
		}



		static std::deque<NotePtr> pentatonicDominantIII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(4));
		}

		static std::shared_ptr<Scale> getPentatonicDominantIII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantIII";
			scale->notes = Scale::pentatonicDominantIII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantIII(std::string note) {
			return Scale::getPentatonicDominantIII(Note::create(note));
		}

		static std::deque<NotePtr> pentatonicDominantIV(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(5));
		}

		static std::shared_ptr<Scale> getPentatonicDominantIV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantIV";
			scale->notes = Scale::pentatonicDominantIV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantIV(std::string note) {
			return Scale::getPentatonicDominantIV(Note::create(note));
		}



		static std::deque<NotePtr> pentatonicDominantbV(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(6));
		}

		static std::shared_ptr<Scale> getPentatonicDominantbV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantbV";
			scale->notes = Scale::pentatonicDominantbV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantbV(std::string note) {
			return Scale::getPentatonicDominantbV(Note::create(note));
		}


		static std::deque<NotePtr> pentatonicDominantV(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(7));
		}

		static std::shared_ptr<Scale> getPentatonicDominantV(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantV";
			scale->notes = Scale::pentatonicDominantV(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantV(std::string note) {
			return Scale::getPentatonicDominantV(Note::create(note));
		}


		static std::deque<NotePtr> pentatonicDominantbVI(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(8));
		}

		static std::shared_ptr<Scale> getPentatonicDominantbVI(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantbVI";
			scale->notes = Scale::pentatonicDominantbVI(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantbVI(std::string note) {
			return Scale::getPentatonicDominantbVI(Note::create(note));
		}




		static std::deque<NotePtr> pentatonicDominantVI(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(9));
		}

		static std::shared_ptr<Scale> getPentatonicDominantVI(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantVI";
			scale->notes = Scale::pentatonicDominantVI(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantVI(std::string note) {
			return Scale::getPentatonicDominantVI(Note::create(note));
		}




		static std::deque<NotePtr> pentatonicDominantbVII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(10));
		}

		static std::shared_ptr<Scale> getPentatonicDominantbVII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantbVII";
			scale->notes = Scale::pentatonicDominantbVII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantbVII(std::string note) {
			return Scale::getPentatonicDominantbVII(Note::create(note));
		}


		static std::deque<NotePtr> pentatonicDominantVII(NotePtr note) {
			return Scale::pentatonicDominant(note->getTransposed(11));
		}

		static std::shared_ptr<Scale> getPentatonicDominantVII(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "pentatonicDominantVII";
			scale->notes = Scale::pentatonicDominantVII(note);
			return scale;
		}

		static std::shared_ptr<Scale> getPentatonicDominantVII(std::string note) {
			return Scale::getPentatonicDominantVII(Note::create(note));
		}
		//===================================================================
#pragma mark - Blues scales
//===================================================================


		static std::deque<NotePtr> blues(NotePtr note) {
			std::deque<NotePtr> scale = Scale::pentatonicMinor(note->copy());
			scale.insert(scale.begin() + 3, Interval::minorFifth(note->copy()));
			return scale;
		}

		static std::shared_ptr<Scale> getBlues(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "blues";
			scale->notes = Scale::blues(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getBlues(std::string note) {
			return Scale::getBlues(Note::create(note));
		}

		//===================================================================
#pragma mark - Chromatic scales
//===================================================================


		static std::shared_ptr<Scale> getChromatic(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "chromatic";
			scale->notes = Scale::chromatic(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getChromatic(std::string note) {
			return Scale::getChromatic(Note::create(note));
		}


		/*
		 Returns the whole note scale starting on note.
		 Example:
		 {{{
		 >>> whole_note("C")
		 ["C", "D", "E", "F#", "G#", "A#"]
		 */
		static std::deque<NotePtr> wholeNote(NotePtr note) {
			std::deque<NotePtr> scale;

			scale.push_back(note->copy());

			NotePtr nCopy = note->copy();
			for (int i = 0; i < 5; i++) {
				nCopy = Interval::majorSecond(nCopy->copy());
				scale.push_back(nCopy->copy());
			}
			return scale;
		}


		static std::shared_ptr<Scale> getWholeNote(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "wholeNote";
			scale->notes = Scale::wholeNote(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getWholeNote(std::string note) {
			return Scale::getWholeNote(Note::create(note));
		}

		//===================================================================
#pragma mark - Ethnic scales
//===================================================================



	/*

	 http://www.jazzguitar.be/bebopscale.html

	 The G Bebop Scale can be played on most chords that are diatonic to the key of C major, but not on the C major chord itself because the F is an avoid note for the C major chord.

	 The Bebop Scale is a dominant scale and has the same function in a key as the Mixolydian scale. It can be played on the dominant and the sub dominant. Our example, the G Bebop Scale, is the dominant of C Major and can be played over G7 and Dm7, giving us a great tool to play over II V I progressions.

	 */

		static std::deque<NotePtr> bebopDominant(NotePtr note) {
			std::deque<NotePtr> bebop = Scale::mixolydian(note->copy());
			NotePtr maj7 = note->getOctaveUp();
			maj7->diminish();
			bebop.insert(bebop.end(), maj7);
			return bebop;
		}

		static std::shared_ptr<Scale> getBebopDominant(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "bebopDominant";
			scale->notes = Scale::bebopDominant(note->copy());
			return scale;
		}


		static std::shared_ptr<Scale> getBebopDominant(std::string note) {
			return Scale::getBebopDominant(Note::create(note));
		}


		/*
		Dorian with added major third
		m13
		https://en.wikipedia.org/wiki/Bebop_scale#Bebop_Dorian_scale
		*/

		static std::deque<NotePtr> bebopMinor(NotePtr note) {
			std::deque<NotePtr> scale = Scale::dorian(note->copy());
			NotePtr majThird = scale[2]->getAugmented();
			scale.insert(scale.begin() + 3, majThird);
			return scale;
		}

		static std::shared_ptr<Scale> getBebopMinor(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "bebopMinor";
			scale->notes = Scale::bebopMinor(note->copy());
			return scale;
		}


		static std::shared_ptr<Scale> getBebopMinor(std::string note) {
			return Scale::getBebopMinor(Note::create(note));
		}


		/*
		 Borg: This is the unique minor flamenco scale,
		 a phrygian with added major third
		 */
		static std::deque<NotePtr> flamenco(NotePtr note) {
			std::deque<NotePtr> scale = Scale::phrygian(note->copy());
			NotePtr majThird = scale[2]->getAugmented();
			scale.insert(scale.begin() + 3, majThird);
			return scale;
		}

		static std::shared_ptr<Scale> getFlamenco(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "flamenco";
			scale->notes = Scale::flamenco(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getFlamenco(std::string note) {
			return Scale::getFlamenco(Note::create(note));
		}

		static std::deque<NotePtr> chromatic(NotePtr note) {
			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			for (int i = 1; i < 12; i++) {
				NotePtr n = note->copy();
				n->set(note->getInt() + i);
				scale.push_back(n);
			}
			return scale;
		}



		/*
		 Japanese scales
		 */

		static std::deque<NotePtr> inSen(NotePtr note) {
			std::deque<NotePtr> scale = Scale::pentatonicMinor(note->copy());
			scale[1] = Interval::minorSecond(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getInSen(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "insen";
			scale->notes = Scale::inSen(note->copy());
			return scale;
		}
		static std::shared_ptr<Scale> getInSen(std::string note) {
			return Scale::getInSen(Note::create(note));
		}

		static std::deque<NotePtr> hirajoshi(NotePtr note) {
			std::deque<NotePtr> scale = Scale::pentatonicMinor(note->copy());
			scale[1] = Interval::minorSecond(note->copy());
			scale[4] = Interval::minorSixth(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getHirajoshi(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "hirajoshi";
			scale->notes = Scale::hirajoshi(note->copy());
			return scale;
		}
		static std::shared_ptr<Scale> getHirajoshi(std::string note) {
			return Scale::getHirajoshi(Note::create(note));
		}

		static std::deque<NotePtr> hindu(NotePtr note) {
			std::deque<NotePtr> scale = Scale::ionian(note->copy());
			scale[5]->diminish();
			return scale;
		}

		static std::shared_ptr<Scale> getHindu(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "hindu";
			scale->notes = Scale::hindu(note->copy());
			return scale;
		}
		static std::shared_ptr<Scale> getHindu(std::string note) {
			return Scale::getHindu(Note::create(note));
		}


		//===================================================================
#pragma mark - Diminished / Augmented scales
//===================================================================

			/*
	 Returns the diminshed scale on note.
	 Example:
	 {{{
	 >>> diminished("C")
	 ['C', 'D', 'Eb', 'F', 'Gb', 'Ab', 'A', 'B']
	 */

		static std::deque<NotePtr> diminished(NotePtr note) {


			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			NotePtr n = note;
			for (int i = 0; i < 3; i++) {
				std::deque<NotePtr> step = wholeStepHalfStep(n);
				scale.insert(scale.end(), step.begin(), step.end());
				n = scale[scale.size() - 1];
			}

			n = Interval::majorSeventh(note->copy());
			scale.push_back(n);


			scale[scale.size() - 2] = Interval::majorSixth(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getDiminished(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "diminished";
			scale->notes = Scale::diminished(note->copy());
			return scale;
		}

		static std::shared_ptr<Scale> getDiminished(std::string note) {
			return Scale::getDiminished(Note::create(note));
		}

		static std::deque<NotePtr> wholeStepHalfStep(NotePtr note) {
			NotePtr maj2 = Interval::majorSecond(note->copy());
			NotePtr min3 = Interval::minorThird(note->copy());
			std::deque<NotePtr> scale;
			scale.push_back(maj2);
			scale.push_back(min3);
			return scale;
		}


		/*
		 Hexatonal
		 http://stormhorn.com/2009/08/24/the-augmented-scale/
		 http://www.javierarau.com/augmented-scale-theory/

		 */

		static std::deque<NotePtr> augmented(NotePtr note) {
			std::deque<NotePtr> scale;
			scale.push_back(note->copy());
			NotePtr n = Interval::minorThird(note->copy());
			scale.push_back(n);
			n = Interval::majorThird(note->copy());
			scale.push_back(n);
			n = Interval::perfectFifth(note->copy());
			scale.push_back(n);
			n = Interval::minorSixth(note->copy());
			scale.push_back(n);
			n = Interval::majorSeventh(note->copy());
			scale.push_back(n);

			return scale;
		}

		static std::shared_ptr<Scale> getAugmented(NotePtr note) {
			if (!Note::isValid(note)) { return 0; }
			std::shared_ptr<Scale> scale = Scale::create();
			scale->name = "augmented";
			scale->notes = Scale::augmented(note->copy());
			return scale;
		}



		static std::shared_ptr<Scale> getAugmented(std::string note) {
			return Scale::getAugmented(Note::create(note));
		}


		//===================================================================
#pragma mark - Helper functions
//===================================================================
		bool isValid() {
			return notes.size() > 1;
		}


		static std::deque<NotePtr> offset(std::deque<NotePtr> orgscale, int amount) {
			if (!(orgscale.size() > amount)) {
#ifdef LOGS
				ofLogWarning() << __FUNCTION__ << " not enough notes" << std::endl;
#endif // LOGS
				return orgscale;
			}
			std::deque<NotePtr> scale;
			std::deque<NotePtr> copyScale;

			for (NotePtr note : orgscale) {
				copyScale.push_back(note->copy());
			}

			sort(copyScale.begin(), copyScale.end(), Note::comparePtr);
			scale.insert(scale.begin(), copyScale.begin() + amount, copyScale.end());

			//now in next octave
			for (int i = 0; i < amount; i++) {
				copyScale[i]->changeOctave(1);
				scale.push_back(copyScale[i]);
			}
			return scale;
		}



		static void setOctave(std::deque<NotePtr> nts, int oct) {
			if (nts.size()) {
				int firstNoteOct = nts[0]->getAbsoluteOctave();
				int diff = oct - firstNoteOct;
				for (NotePtr note : nts) {
					note->changeOctave(diff);
				}
			}
		}

		void setOctave(int oct) {
			Scale::setOctave(notes, oct);
		}

		int getOctave() {
			if (notes.size()) {
				return notes[0]->getAbsoluteOctave();
			}
			else {
				return 0;
			}
		}



		static void changeOctave(std::deque<NotePtr>& _notes, int diff) {
			for (int i = 0; i < _notes.size(); i++) {
				_notes[i]->changeOctave(diff);
			}
		}

		void changeOctave(int diff) {
			for (int i = 0; i < notes.size(); i++) {
				notes[i]->changeOctave(diff);
			}
		}


		void octaveUp() {
			changeOctave(1);
		}

		std::shared_ptr<Scale> getOctaveUp() {
			std::shared_ptr<Scale> n = copy();//copy
			n->changeOctave(1);
			return n;
		}

		void octaveDown() {
			changeOctave(-1);
		}

		std::shared_ptr<Scale> getOctaveDown() {
			std::shared_ptr<Scale> n = copy();//copy
			n->changeOctave(-1);
			return n;
		}



		NotePtr getFirst() {
			if (isValid()) {
				return notes[0];
			}
#ifdef LOGS
			else {
				ofLogWarning() << "Scale::getFirst has no notes" << std::endl;
			}
#endif // LOGS
			return nullptr;
		}

		NotePtr getLast() {
			if (isValid()) {
				return notes[notes.size() - 1];
			}
#ifdef LOGS
			else {
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return nullptr;
		}


		NotePtr getRoot() {
			return getFirst();
		}

		/*
		Return major or minor third.
		If contains both, minor will be returned
		*/
		NotePtr getThird() {
			if (isValid()) {
				NotePtr root = getRoot();
				NotePtr third;
				for (NotePtr note : notes) {
					int halfTones = Interval::measure(root, note);
					if (halfTones == 3 || halfTones == 4) {
						return note->copy();
					}
				}
#ifdef LOGS
				ofLogWarning() << "Scale does not contain a third" << std::endl;
#endif // LOGS
			}
#ifdef LOGS
			else {
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return nullptr;
		}

		NotePtr getSeventh() {
			if (isValid()) {
				NotePtr root = getRoot();
				NotePtr seventh;
				for (NotePtr note : notes) {
					int halfTones = Interval::measure(root, note);
					if (halfTones == 10 || halfTones == 11) {
						return note->copy();
					}
				}
#ifdef LOGS
				ofLogWarning() << "Scale does not contain a seventh" << std::endl;
#endif // LOGS
			}
#ifdef LOGS
			else
			{
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return nullptr;
		}

		/*
		Accepts scale degree starting on 0.
		*/
		NotePtr getNote(int i) {
			if (isValid()) {
				int oct = 0;
				if (i < 0) {
					oct = -ceil((float)-i / (float)notes.size());
					i = (notes.size() + i) % notes.size();
				}
				else if (i >= notes.size()) {
					oct = floor((float)i / (float)notes.size());
					i = i % notes.size();
				}


				NotePtr n = notes[i]->copy();
				n->changeOctave(oct);
				return n;

			}
#ifdef LOGS
			else
			{
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return nullptr;
		}

		/*
		Get scale degree starting on 0.
		Note: Does not distinguish between scales with more or less than 7 notes.
		*/

		int getDegree(NotePtr n) {
			if (isValid()) {
				for (int i = 0; i < notes.size(); i++) {
					if (n->getInt() % 12 == notes[i]->getInt() % 12) {
						return i;
					}
				}
				std::cout << "Note " << n << " not found in scale " << notes[0] << " " << name << std::endl;

			}
#ifdef LOGS
			else
			{
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return -1;//not found
		}

		NotePtr getClosestNote(NotePtr note, bool ifNotInScaleSelectNextHigher = true) {
			if (isValid()) {
				int startNote = note->getInt();
				int startOct = note->getAbsoluteOctave();

				std::shared_ptr<Scale> stack = copy();
				stack->setOctave(startOct);

				while (stack->getNote(0)->getInt() > startNote) {
					stack->octaveDown();
				}
				bool found = false;
				int currNote = 0;

				while (!found) {
					if (stack->getNote(currNote)->getInt() == note->getInt()) {
						found = true;
						return stack->getNote(currNote)->copy();
					}

					if (stack->getNote(currNote)->getInt() > note->getInt()) {
						//just missed it
						if (ifNotInScaleSelectNextHigher) {
							found = true;
							return stack->getNote(currNote)->copy();
						}
						else {
							found = true;
							return stack->getNote(currNote - 1)->copy();
						}
					}
					currNote++;
				}
			}
#ifdef LOGS
			else
			{
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return note->copy();
		}


		/*
		Useful eg. for finding the third closest to C3.
		*/
		NotePtr getDegreeClosestToNote(int degree, NotePtr note) {
			if (isValid()) {
				int startNote = note->getInt();
				int startOct = note->getAbsoluteOctave();


				NotePtr searchNote = Scale::getNote(degree);
				searchNote->setOctave(startOct);
				NotePtr above = searchNote->getOctaveUp();
				NotePtr below = searchNote->getOctaveDown();

				int dif1 = abs(startNote - searchNote->getInt());
				int dif2 = abs(startNote - above->getInt());
				int dif3 = abs(startNote - below->getInt());

				if (dif1 <= dif2 && dif1 <= dif3) {
					return searchNote;
				}
				else if (dif2 <= dif1 && dif2 <= dif3) {
					return above;
				}
				else {
					return below;
				}

			}
#ifdef LOGS
			else
			{
				ofLogWarning() << "Scale invalid" << std::endl;
			}
#endif // LOGS
			return note->copy();
		}

		//===================================================================
#pragma mark - SCALES FOR CHORDS
//===================================================================
		/*
		 Determines the kind of scale. Can recognize all the diatonic modes and \
		 the minor scales.
		 Example:
		 {{{
		 >>> determine(["C", "D", "E", "F", "G", "A", "B"])
		 'C ionian'
		 */
		 /*

	static std::string determine(std::deque<NotePtr> scale){


	 possible_result = [
						["ionian",
						 ["major second", "major third", "perfect fourth",
						  "perfect fifth", "major sixth", "major seventh"]],
						["dorian",
						 ["major second", "minor third", "perfect fourth",
						  "perfect fifth", "major sixth", "minor seventh"]],
						["phrygian",
						 ["minor second", "minor third", "perfect fourth",
						  "perfect fifth", "minor sixth", "minor seventh"]],
						["lydian",
						 ["major second", "major third", "major fourth",
						  "perfect fifth", "major sixth", "major seventh"]],
						["mixolydian",
						 ["major second", "major third", "perfect fourth",
						  "perfect fifth", "major sixth", "minor seventh"]],
						["aeolian",
						 ["major second", "minor third", "perfect fourth",
						  "perfect fifth", "minor sixth", "minor seventh"]],
						["locrian",
						 ["minor second", "minor third", "perfect fourth",
						  "minor fifth", "minor sixth", "minor seventh"]],
						["natural minor",
						 ["major second", "minor third", "perfect fourth",
						  "perfect fifth", "minor sixth", "minor seventh"]],
						["harmonic minor",
						 ["major second", "minor third", "perfect fourth",
						  "perfect fifth", "minor sixth", "major seventh"]],
						["melodic minor",
						 ["major second", "minor third", "perfect fourth",
						  "perfect fifth", "major sixth", "major seventh"]],
						]

	 tonic = scale[0]
	 n = 0

// -- Describing the algorithm:
// Filter out all the wrong answers in possible_result
		 for note in scale[1:]:
// 1. Determine the interval
		 intval = Interval.determine(tonic, note)

		 a=0
		 temp = []

// 2. Go through possible_result and add it to temp if
// it's a hit, do nothing otherwise
		 for x in possible_result:
		 if x[1][n] == intval:
		 temp.append(x)
		 n += 1

// 3. Set possible_result to temp
		 possible_result = temp

// Get the results from possible_result and return
		 res = []
		 for x in possible_result:
		 res.append(scale[0] + " " + x[0])
		 return res
*/

/*
 Get possible scales for a given chord, eg. maj7

 */


		static std::vector<std::string> getScalesForChord(std::string chordSymbol) {
			std::string str = ChordScaleLookup[chordSymbol];
			return utils::splitString(str, ",");

		}


		static std::vector<std::shared_ptr<Scale>> getScalesForChord(ChordPtr chord) {
			std::vector<std::shared_ptr<Scale>> scalesInKey;
			std::string str = ChordScaleLookup[chord->getChordSymbol()];

			if (str == "") {
				std::cout << "getScalesForChord found nothing in ChordScaleLookup for " << chord->getChordSymbol() << std::endl;
				return scalesInKey;
			}
			std::vector<std::string> scales = utils::splitString(str, ",");//scales that go with this symbol
			//how to consider bass and poly...baah..

			std::shared_ptr<Scale> s;
			for (int i = 0; i < scales.size(); i++) {
				s = Scale::getScaleFromString(scales[i], chord->getRoot());
				s->setOctave(3);
				scalesInKey.push_back(s);
			}
			return scalesInKey;
		}

		static std::shared_ptr<Scale> getScaleFromString(std::string scaleName, NotePtr n) {

			std::shared_ptr<Scale> s;
			if (ScaleLookup()[scaleName]) {
				s = (*ScaleLookup()[scaleName])(n);
			}
			else {
				std::cout << "Scale::getScaleFromString not found for chord: " << scaleName << std::endl;
			}

			return s;

		}



		//convenience

		static void print(std::deque<Note> notes) {


			std::cout << "[ ";
			for (int i = 0; i < notes.size(); i++) {
				std::cout << notes[i];
				if (i < notes.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << " ]" << std::endl;
		}



		static void print(std::deque<NotePtr> notes) {


			std::cout << "[ ";
			for (int i = 0; i < notes.size(); i++) {
				std::cout << notes[i];
				if (i < notes.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << " ]" << std::endl;
		}


		static void print(std::vector<std::string> symbols) {

			std::cout << "[ ";
			for (int i = 0; i < symbols.size(); i++) {
				std::cout << symbols[i];
				if (i < symbols.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << " ]" << std::endl;


		}


		/*
		 //fix
		static void print(Scale  s){
			std::cout<<s;
		}*/

		//give access to private parts
		friend std::ostream& operator<<(std::ostream& os, const Scale& s);
		friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Scale>& s);


	private:
		//===================================================================
#pragma mark - Scale function hash map
//===================================================================


		static ScaleFunctionLookup ScaleLookup() {
			static ScaleFunctionLookup _scaleLookup = {
				 {"diatonic",&Scale::getDiatonic},
				 {"ionian",&Scale::getIonian},
				 {"dorian",&Scale::getDorian},
				 {"phrygian",&Scale::getPhrygian},
				 {"lydian",&Scale::getLydian},
				 {"mixolydian",&Scale::getMixolydian},
				 {"aeolian",&Scale::getAeolian},
				 {"locrian",&Scale::getLocrian},
				 {"halfDiminished",&Scale::getLocrian},
				 {"pentatonicMinor",&Scale::getPentatonicMinor},
				 {"pentatonicMinorbII",&Scale::getPentatonicMinorbII},
				 {"pentatonicMinorII",&Scale::getPentatonicMinorII},
				 {"pentatonicMinorbIII",&Scale::getPentatonicMinorbIII},
				 {"pentatonicMinorIII",&Scale::getPentatonicMinorIII},
				 {"pentatonicMinorIV",&Scale::getPentatonicMinorIV},
				 {"pentatonicMinorbV",&Scale::getPentatonicMinorbV},
				 {"pentatonicMinorV",&Scale::getPentatonicMinorV},
				 {"pentatonicMinorbVI",&Scale::getPentatonicMinorbVI},
				 {"pentatonicMinorVI",&Scale::getPentatonicMinorVI},
				 {"pentatonicMinorbVII",&Scale::getPentatonicMinorbVII},
				 {"pentatonicMinorVII",&Scale::getPentatonicMinorVII},
				 {"pentatonicMajor",&Scale::getPentatonicMajor},
				 {"pentatonicDominant",&Scale::getPentatonicDominant},
				 {"pentatonicDominantbII",&Scale::getPentatonicDominantbII},
				 {"pentatonicDominantII",&Scale::getPentatonicDominantII},
				 {"pentatonicDominantbIII",&Scale::getPentatonicDominantbIII},
				 {"pentatonicDominantIII",&Scale::getPentatonicDominantIII},
				 {"pentatonicDominantIV",&Scale::getPentatonicDominantIV},
				 {"pentatonicDominantbV",&Scale::getPentatonicDominantbV},
				 {"pentatonicDominantV",&Scale::getPentatonicDominantV},
				 {"pentatonicDominantbVI",&Scale::getPentatonicDominantbVI},
				 {"pentatonicDominantVI",&Scale::getPentatonicDominantVI},
				 {"pentatonicDominantbVII",&Scale::getPentatonicDominantbVII},
				 {"pentatonicDominantVII",&Scale::getPentatonicDominantVII},
				 {"melodicMinor",&Scale::getMelodicMinor},
				 {"melodicMinorII",&Scale::getMelodicMinorII},
				 {"melodicMinorIII",&Scale::getMelodicMinorIII},
				 {"augmented",&Scale::getAugmented},
				 {"melodicMinorII",&Scale::getMelodicMinorII},
				 {"melodicMinorIII",&Scale::getMelodicMinorIII},
				 {"melodicMinorIV",&Scale::getMelodicMinorIV},
				 {"melodicMinorV",&Scale::getMelodicMinorV},
				 {"melodicMinorVI",&Scale::getMelodicMinorVI},
				 {"melodicMinorVII",&Scale::getMelodicMinorVII},
				 {"naturalMinor",&Scale::getNaturalMinor},
				 {"harmonicMinor",&Scale::getHarmonicMinor},
				 {"flamenco",&Scale::getFlamenco},
				 {"diminished",&Scale::getDiminished},
				 {"bebopDominant",&Scale::getBebopDominant},
				 {"bebopMinor",&Scale::getBebopMinor},
				 {"blues",&Scale::getBlues},
				 {"lydianDiminished",&Scale::getLydianDiminished},
				 {"lydianDominant",&Scale::getLydianDominant},
				 {"inSen",&Scale::getInSen},
				 {"hirajoshi",&Scale::getHirajoshi},
				 {"hindu",&Scale::getHindu},
				 {"chromatic",&Scale::getChromatic},
				 {"wholenote",&Scale::getWholeNote}
			};


			return _scaleLookup;

		}
	};


	typedef std::shared_ptr<Scale> ScalePtr;

	//this overloads the std::cout stream with useful output data
	//corresponding friend function above, note: inside class
	inline std::ostream& operator<<(std::ostream& os, Scale& s) {
		if (s.size()) {
			os << "Scale " << s.notes[0]->getShorthand() << " " << s.name << " [ ";
			for (int i = 0; i < s.notes.size(); i++) {
				os << s.notes[i];
				if (i < s.notes.size() - 1) {
					os << ", ";
				}
			}
			os << " ]" << std::endl;
		}
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, ScalePtr& s) {
		if (Scale::isValid(s)) {
			os << "Scale " << s->notes[0]->getShorthand() << " " << s->name << " [ ";
			for (int i = 0; i < s->notes.size(); i++) {
				os << s->notes[i];
				if (i < s->notes.size() - 1) {
					os << ", ";
				}
			}
			os << " ]" << std::endl;
		}
		else {
			os << "Invalid scale" << std::endl;
		}
		return os;
	}



};//namespace
#endif
