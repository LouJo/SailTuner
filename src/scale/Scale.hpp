#ifndef _SCALE_HPP
#define _SCALE_HPP

/**
 * Note recognition within a temperament
 */
class Scale {
	private:
	static const int nbNote = 12;
	static const int cmpOctave = 4;
	static const int defaultLa = 440;

	static const char *noteNames[nbNote];

	double noteFreq[nbNote], actualNoteFreq[nbNote];
	double actualLa, actualFactor;
	double actualRange[2]; // freq range for default octave

	/// update scale after temperament or default la change
	void updateScale();

	/**
	 * give the half-tone frequency factor
	 * beetween note and previous or next
	 */
	double halfToneFactor(int note, int sign);

	/// find octave and update freq in range
	int findOctave(double &freq);

	public:
	Scale();
	~Scale() {}

	void ConstructEqualTemperament();

	double GetLa();
	void SetLa(double la);

	/**
	 * Find nearest note, octave, and deviation
	 */
	int FindNote(double freq, int &octave, double &deviation);


	static const char * NoteName(int note);
};

#endif
