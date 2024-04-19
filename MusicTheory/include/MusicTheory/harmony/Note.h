/*
 *  Note.h
 *  MusicTheory
 *
 *  Created by Andreas Borg on 28/01/2013
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Note
#define _Note

#include <map>
#include <memory>

#include <boost/regex.hpp>

#include <mathfu/vector.h>

namespace MusicTheory {




	typedef struct Dynamics {
		int velocity = 100;
		int channel = 1;
		int volume = 1;

	}Dynamics;

	static const  std::string DiatonicAugNames = "C,C#,D,D#,E,F,F#,G,G#,A,A#,B";
	static const  std::string DiatonicDimNames = "C,Db,D,Eb,E,F,Gb,G,Ab,A,Bb,B";
	static const  std::string NoteDictionary = "C,,D,,E,F,,G,,A,,B";




	//static const  std::string DiatonicAugNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	//static const  std::string DiatonicDimNames[] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};




	class Note : public std::enable_shared_from_this<Note> {

	public:


		std::string name = "";
		//https://forum.ableton.com/viewtopic.php?f=1&t=148636
		//midi octave ranges are not standardised. I've adjusted to Ableton which begins on -2 = 0
		//middle C = 60 = C3 on Ableton
		int octave = 3;
		Dynamics dynamics;




		/*
		 Name can be eg. C, C-1, 12
		[[deprecated("Use Note::create(...)")]]
		 */

		Note(std::string _name = "C", int _oct = 3, Dynamics _dyn = Dynamics()) {
			std::transform(_name.begin(), _name.begin(), _name.begin(), ::toupper);
			set(_name, _oct, _dyn);


		};



		//===================================================================
#pragma mark - FACTORY METHODS
//===================================================================

	//factory methods
		std::shared_ptr<Note> copy() {
			if (isValid()) {
				return std::shared_ptr<Note>(new Note(*this));//copy
			}
			else 
			{
#ifdef LOGS
				ofLogError() << __FUNCTION__ << " trying to copy empty note" << endl;
#endif // LOGS
				return nullptr;
			}
		}

		static std::shared_ptr<Note> create(std::string _name = "C", int _oct = 3, Dynamics _dyn = Dynamics()) {
			if (Note::isValidName(_name)) 
			{
				return std::shared_ptr<Note>(new Note(_name, _oct, _dyn));//new
			}
			else 
			{
				return nullptr;
			}
		}

		//===================================================================
#pragma mark - INSTANCE METHODS
//===================================================================

		bool isValid() {
			return Note::isValidName(name);
		}


		void set(std::string _name = "C", int _oct = 3, Dynamics _dyn = Dynamics()) {
			if (!Note::isValidName(_name)) {
				return;
			}


			/*
			_name = ofToUpper(_name.substr(0,1))+_name.substr(1);

			//extract embedded oct eg. D#6
			if(ofIsstd::stringInstd::std::string(_name, "#") && _name.size()>2){
				std::vector<std::string> div = utils::splitString(_name,"#");
				if(isdigit(div[1][0])){
					_oct = std::stoi(div[1]);
				}
			}else if(ofIsstd::stringInstd::std::string(_name, "b") && _name.size()>2){
				std::vector<std::string> div = utils::splitString(_name,"b");
				if(isdigit(div[1][0])){
					_oct = std::stoi(div[1]);
				}
			}else if(_name.size()>1){
				if(isdigit(_name[_name.size()-1])){
					_oct = std::stoi(_name.substr(1,_name.size()-1));
				}

			}
			*/
			name = _name;
			octave = _oct;

			fromShorthand(_name);
			dynamics = _dyn;
		}

		void set(int note, Dynamics _dyn = Dynamics(), bool absolute = true) {
			if (!Note::isValidName(name)) {
				return;
			}

			std::string n[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

			if (absolute) {
				name = n[note % 12];
				octave = (note / 12) - 2;
			}
			else {
				if (note < 0 || note>11) {
#ifdef LOGS
					ofLog() << note << " outside note range 0-11" << endl;
#endif // LOGS
					return;
				}
				else {
					name = n[note];
					octave = 3;
				}

			}





			dynamics = _dyn;
		}



		void empty() {
			name = "";
			octave = 0;
			dynamics = Dynamics();
		}
		/*

		 Augments the note itself.

		 This produces double and triple etc. augmentations on repetition, eg. A###, which is hard to read.
		 Theorists claim it is the correct way of notating scales to avoid clashing with other non-altered notes
		 in the same scale, eg. writing A### as C would clash with the natural C.

		 Use getDiatonicName to output A### as C.
		 */
		void augment(int i = 1) {
			std::string last;
			if (name.size() == 1) {
				last = name;
			}
			else {
				last = name.substr(name.size() - 1, 1);
			}
			if (last == "b") {
				//if already dim, just remove b
				name = name.substr(0, name.size() - 1);

			}
			else {
				for (int ii = 0; ii < i; ii++) {
					name += "#";
				}
			}
		}

		std::shared_ptr<Note> getAugmented(int i = 1) {
			std::shared_ptr<Note> n = copy();
			n->augment(i);
			return n;
		}

		void diminish(int i = 1) {
			std::string last;
			if (name.size() == 1) {
				last = name;
			}
			else {
				last = name.at(name.size() - 1);
			}

			if (last == "#") {
				name = name.substr(0, name.size() - 1);

			}
			else {
				for (int ii = 0; ii < i; ii++) {
					name += "b";
				}
			}

		}
		std::shared_ptr<Note> getDiminished(int i = 1) {
			//NotePtr n = *this;
			std::shared_ptr<Note> n = std::shared_ptr<Note>(new Note(*this));
			n->diminish(i);
			return n;
		}

		/*
		 Removes any accidentals on this note, eg. Bb to B
		 */
		void naturalise() {
			set(this->getUnaltered(), octave, dynamics);
		}
		/*
		 Returns a natural copy
		 */
		std::shared_ptr<Note> getNatural() {
			std::shared_ptr<Note> n = std::shared_ptr<Note>(new Note(*this));
			n->naturalise();
			return n;
		}


		void changeOctave(int diff, bool limit = false) {
			octave += diff;

			if (limit) {
				octave = std::clamp(octave, -2, 8);
			}

		}

		std::shared_ptr<Note> setOctave(int oct, bool limit = false) {
			//Ableton starts at -2
			octave = oct;

			if (limit) {
				octave = std::clamp(octave, -2, 8);
			}
			return shared_from_this();
		}

		int getOctave() {
			return octave;
		}

		void octaveUp() {
			changeOctave(1);
		}

		std::shared_ptr<Note> getOctaveUp() {
			std::shared_ptr<Note> n = copy();
			n->changeOctave(1);
			return n;
		}

		void octaveDown() {
			changeOctave(-1);
		}

		std::shared_ptr<Note> getOctaveDown() {
			std::shared_ptr<Note> n = copy();
			n->changeOctave(-1);
			return n;
		}

		void limitToOctaves(int minOct, int maxOct) {
			if (getOctave() < minOct) {
				changeOctave(minOct - getOctave());
			}

			if (getOctave() > maxOct) {
				changeOctave(maxOct - getOctave());
			}
		}

		std::shared_ptr<Note> getLimitToOctaves(int minOct, int maxOct) {
			std::shared_ptr<Note> n = copy();
			n->limitToOctaves(minOct, maxOct);
			return n;

		}

		std::shared_ptr<Note> getNearestOctave(std::shared_ptr<Note> ref) {
			std::shared_ptr<Note> n = copy();
			int mid = ref->getInt();
			int currDist = n->getInt() - mid;
			if (currDist > 0) {
				//above
				int octDownDist = std::abs(n->getOctaveDown()->getInt() - mid);
				bool cont = true;
				while (octDownDist < currDist && cont) {
					currDist = n->getInt() - mid;
					n->octaveDown();
					octDownDist = std::abs(n->getInt() - mid);
					if (octDownDist >= currDist) {
						//doesn't make a difference or same
						n->octaveUp();
						cont = false;
					}
				}
			}
			else {
				//below, pitch up
				int octUpDist = std::abs(n->getOctaveUp()->getInt() - mid);
				bool cont = true;


				currDist = std::abs(currDist);
				while (octUpDist < currDist && cont) {
					currDist = std::abs(n->getInt() - mid);
					n->octaveUp();
					octUpDist = std::abs(n->getInt() - mid);
					if (octUpDist >= currDist) {
						//doesn't make a difference or same
						n->octaveDown();
						cont = false;
					}
				}
			}
			return n;
		}

		void toMinor() {
			//name = Notes::toMinor(name);

			//sth = Interval.h::sixth(note[0], 'C')
		   //sth = diatonic.interval(key, note, 5)

			//return augment_or_diminish_until_the_interval_is_right(note, sth, 9)

		}


		void toMajor() {
			// name = Notes::toMajor(name);
		}

		void removeRedundantAccidentals() {
			// name = Notes::removeRedundantAccidentals(name);
		}
		/*
		 Transposes the note up or down the interval.
		 >>> a = Note("A")
		 >>> a.transpose(3)
		 >>> a
		 'C#-5'
		 >>> a.transpose(-3)
		 >>> a
		 'A-4'

		 Note: Removes accidentals (but adds # if necessary)
		 */

		void transpose(int interval) {
			int dif = toInt() + interval;
			set(dif, dynamics);
		}


		std::shared_ptr<Note> getTransposed(int interval) {
			std::shared_ptr<Note> n = copy();
			n->transpose(interval);
			return n;
		}
		/*
		 Returns the number of semitones between this Note and the other.
		{{{
			>>> Note("C").measure(Note("D"))
			2
			>>> Note("D").measure(Note("C"))
			-2
		}}}
		 */

		int measure(Note other) {
			return other.toInt() - toInt();


		}
		/*
		 If relative, returns value from 0-11 where C = 0.
		 Else considers octave.
		 */
		int  toInt(bool relative = false) const {
			// 

			if (!&name) {
#ifdef LOGS
				cout << "Warning: No name" << endl;
#endif // LOGS
				return 0;
			}
			//root note
			std::string root = name.substr(0, 1);
			auto nNames = utils::splitString(NoteDictionary, ",");
			std::vector<std::string>::iterator it = find(nNames.begin(), nNames.end(), root);
			int uid = it - nNames.begin();

			int numberOfAccidentals = utils::getNumberOfAccidentals(name);

			if (relative) {
				int val = uid + numberOfAccidentals;
				return val % 12;
			}
			else {
				return (octave + 2) * 12 + uid + numberOfAccidentals;
			}

		}

		int  getInt() const {
			return toInt();

		}


		//ableton


		//F5//89 = 8
		//E5//88 = 7
		//D#5//87 = 6#
		//D5//86 = 6
		//C#5//85 = 5#
		//C5//84 = 5
		//B4//83 = 4
		//A#4//82 = 3#
		//A4//81 = 3
		//G#4//80 = 2#
		//G4//79 = 2
		//(Gb4//78 = 2b)
		//F#4//78 = 1#
		//F4//77 = 1
		//E4//76 = 0
		//(Db4//75 = 0b)
		//D#4//75 = -1#
		//D4//74 = -1
		//(Db4//73 = -1b)
		//C#4//73 = -2#
		//C4//72 = -2

		//B3//71 = -3

		std::map<int, mathfu::Vector<float, 2>> midToClef;


		mathfu::Vector<float, 2> toTrebleClef()
		{
			if (!midToClef.size()) {
				midToClef[11] = mathfu::Vector<float, 2>(4, 0);//B
				midToClef[10] = mathfu::Vector<float, 2>(3, 1);//A#
				midToClef[9] = mathfu::Vector<float, 2>(3, 0);//A
				midToClef[8] = mathfu::Vector<float, 2>(2, 1);//G#
				midToClef[7] = mathfu::Vector<float, 2>(2, 0);//G
				midToClef[6] = mathfu::Vector<float, 2>(1, 1);//F#
				midToClef[5] = mathfu::Vector<float, 2>(1, 0);//F
				midToClef[4] = mathfu::Vector<float, 2>(0, 0);//E
				midToClef[3] = mathfu::Vector<float, 2>(-1, 1);//D#
				midToClef[2] = mathfu::Vector<float, 2>(-1, 0);//D
				midToClef[1] = mathfu::Vector<float, 2>(-2, 1);//C#
				midToClef[0] = mathfu::Vector<float, 2>(-2, 0);//C
			}

			int mid = getInt();
			int oct = floor(mid / 12);
			int trans = oct - 5;//C3 is 5th oct cause it starts on -2
			int nn = mid % 12;
			mathfu::Vector<float, 2> line = midToClef[nn];
			line.x += 7 * trans;
			return line;

		}

		//Todo
		mathfu::Vector<float, 2> toBassClef() {

		}


		//not sure this is used...from Mingus
		std::string intToNote(int note) {
			if (note < 0 || note>11) {
#ifdef LOGS
				ofLog() << note << " outside note range 0-11" << endl;
#endif // LOGS
				return "";
			}

			std::string n[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
			return n[note];

		}




		/*
		 Returns the Note in Hz. The `standard_pitch` argument can be used \
		to set the pitch of A-4, from which the rest is calculated.
		 Not done.
		 */
		float toHertz(int standardPitch = 440) {
			float diff = toInt() - 57.0;
			return pow(2, (diff / 12.0)) * 440;
		}


		/*
		 Sets the Note name and pitch, calculated from the `hertz` value. \
		 The `standard_pitch` argument can be used to set the pitch of A-4, from \
		 which the rest is calculated.*/

		int fromHertz(float hertz, int standardPitch = 440) {
			/*value = log(float(hertz) / standardPitch, 2) * 12 + Note("A").toInt();
			name = notes.int_to_note(int(value) % 12);
			octave = int(value / 12) + 4;*/
			return 0;
		}


		/*

		 Eg. C#-5

		 */
		std::string getShorthand() {

			int adjustedOct = floor(getInt() / 12) - 2;
			std::string str = getDiatonicName() + "" + std::to_string(getAbsoluteOctave());
			return str;
		}

		/*
		getDiatonicName will return C for B#, which is in next octave from B#
		hence, should take octave from int intead
		*/
		int getAbsoluteOctave() {
			int diatonicOct = floor(getInt() / 12) - 2;
			return diatonicOct;
		}

		std::string getName() {
			return name;
		}



		std::string getDiatonicName() const {

			if (name.size() == 1) {
				return name;
			}
			//translate C### to D#

		   //get num of accidentals
			int diff = utils::getNumberOfAccidentals(name);


			//root note
			std::string root = name.substr(0, 1);


			if (diff > 0) {
				std::vector<std::string> augNames = utils::splitString(DiatonicAugNames, ",");
				std::vector<std::string>::iterator it = find(augNames.begin(), augNames.end(), root);
				int uid = it - augNames.begin();
				int skip = (diff + uid) % 12;//this returns correct id in note sequence
				return augNames[skip];
			}
			else if (diff < 0) {
				std::vector<std::string> dimNames = utils::splitString(DiatonicDimNames, ",");
				std::vector<std::string>::iterator it = find(dimNames.begin(), dimNames.end(), root);
				int uid = it - dimNames.begin();
				//find root and go down the number of b's
				int skip = (uid + 12 + diff) % 12;//this returns correct id in note sequence
				return dimNames[skip];
			}
			else {
				//#b equalled out
				return root;
			}
		}


		//Some std::string functions
		/*
		 Returns eg. ## from B##
		 */
		std::string getAccidentals() {
			return name.substr(1, name.size() - 1);
		}
		/*
		 Returns natural std::string name eg. B from B##
		 */
		std::string getUnaltered() {
			return name.substr(0, 1);
		}

		//int getNumberOfSharps() const
		//{
		//	return std::ranges::count(name, '#');
		//}

		//int getNumberOfFlats() const
		//{
		//	return std::ranges::count(name, 'b');
		//}

		//int getNumberOfAccidentals() const
		//{
		//	std::string cleanName = getDiatonicName();
		//	int augs = getNumberOfSharps();
		//	int dims = getNumberOfFlats();
		//	return augs - dims;
		//}


		//===================================================================
#pragma mark - STATIC METHODS
//===================================================================



		static bool isValid(std::shared_ptr<Note>n) {
			if (!n) {
				return false;
			}
			else {
				return n->isValid();
			}
		}

		static bool isValidName(std::string _name)
		{
			boost::regex rex{ "[a-gA-G][[b|#]*]?[\-]?[0-9]?" };
			boost::smatch match;
			if (boost::regex_search(_name, match, rex))
			{
				return true;
			}
			else
			{
#ifdef LOGS
				ofLogError() << "The std::string " << _name << " is not a valid representation of a note" << endl;
#endif // LOGS
				return false;
			}
		}

		static std::shared_ptr<Note> fromInt(int val) {
			if (val < 0) {
#ifdef LOGS
				ofLogError() << "Negative note value not allowed " << val << endl;
#endif // LOGS
				return 0;
			}
			int relVal = val % 12;
			int oct = floor(val / 12) - 2;

			std::string n[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

			std::shared_ptr<Note> note = Note::create(n[relVal]);
			note->setOctave(oct);
			return note;

		}

		static int getNoteId(std::deque<Note> const& list, Note const& n, bool strict = false) {

			for (int i = 0; i < list.size(); i++) {
				if (n.name == list[i].name) {
					return i;
				}
			}

			return -1;
		}

		static int getNoteId(std::deque<std::shared_ptr<Note> > list, std::shared_ptr<Note> n, bool strict = false) {

			for (int i = 0; i < list.size(); i++) {
				if (n->name == list[i]->name) {
					return i;
				}
			}

			return -1;
		}


		//http://stackoverflow.com/questions/6039567/const-member-function
		static bool compare(const Note& a, const Note& b) {
			// cout<<a.toInt() <<" "<< b.toInt()<<endl;
			return (a.toInt() < b.toInt());
		}

		static bool comparePtr(const std::shared_ptr<Note>& a, const std::shared_ptr<Note>& b) {
			return (a->toInt() < b->toInt());
		}




		//give access to private parts
		friend std::ostream& operator<<(std::ostream& os, const Note& n);

		friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Note>& n);

		//===================================================================
#pragma mark -		PRIVATE METHODS
//===================================================================

	private:

		/*
		Convert from
		E
		c#3
		Bbb4
		or
		c#-1
		Bbb-2

		Because Ableton is using negative -2 the use of the short dash
		cannot be used to divide between note name and octave.
		 */

		void fromShorthand(std::string shorthand) {

			//is octave appended?
			name = std::string("");
			if (std::ranges::count(shorthand, '-'))
			{
				std::vector<std::string>soct = utils::splitString(shorthand, "-");
				if (isdigit(soct[1][0]))
				{
					octave = -std::stoi(soct[1]);
				}

				name.push_back((unsigned char)::toupper(soct[0][0]));
				name += soct[0].substr(1, std::string::npos);
			}
			else if (isdigit(shorthand.back()))
			{
				octave = std::stoi(shorthand.substr(shorthand.size() - 1, 1));
				name.push_back((unsigned char)::toupper(shorthand[0]));
				name += shorthand.substr(1, shorthand.size() - 2);
			}
			else
			{
				name.push_back((unsigned char)::toupper(shorthand[0]));
				name += shorthand.substr(1, std::string::npos);
			}


			/*

			int diff = getNumberOfAccidentals();
			transpose(diff);
			*/


			/*
			//extract embedded oct eg. D#6
			if(ofIsstd::stringInstd::std::string(_name, "#") && _name.size()>2){
				std::vector<std::string> div = utils::splitString(_name,"#");
				if(isdigit(div[1][0])){
					_oct = std::stoi(div[1]);
				}
			}else if(ofIsstd::stringInstd::std::string(_name, "b") && _name.size()>2){
				std::vector<std::string> div = utils::splitString(_name,"b");
				if(isdigit(div[1][0])){
					_oct = std::stoi(div[1]);
				}
			}else if(_name.size()>1){
				if(isdigit(_name[_name.size()-1])){
					_oct = std::stoi(_name.substr(1,_name.size()-1));
				}

			}
			*/

		}


	};//class







	typedef std::shared_ptr<Note> NotePtr;

	typedef NotePtr N;//short enough?

	//this overloads the cout stream with useful output data
	//corresponding friend function above, note: inside class
	inline std::ostream& operator<<(std::ostream& os, Note& n) {
		//name contains accidentals info getDiatonicName() doesn't
		os << "Note " << n.getShorthand() << " (" << n.name << " " << n.getInt() << ")";
		//os <<"Note "<< n.getShorthand() << " (vel: " << n.dynamics.velocity<<")";
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, NotePtr& n) {
		if (Note::isValid(n)) {
			os << "Note " << n->getShorthand() << " (" << n->name << " " << n->getInt() << ")";
		}
		else {
			os << "Note invalid";
		}
		return os;
	}



}//namespace
#endif

