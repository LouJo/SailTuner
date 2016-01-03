#include <QAudioRecorder>
#include <QAudioProbe>

#include <fstream>

#include "audio/LinearFilter.hpp"
#include "audio/ZeroCross.hpp"
#include "scale/Scale.hpp"

class Tuner : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool running READ GetRunning WRITE SetRunning NOTIFY runningChanged)
	Q_PROPERTY(double freq READ GetFreq NOTIFY freqChanged)
	Q_PROPERTY(double deviation READ GetDeviation NOTIFY deviationChanged)
	Q_PROPERTY(int note READ GetNote NOTIFY noteChanged)
	Q_PROPERTY(int octave READ GetOctave NOTIFY octaveChanged)
	Q_PROPERTY(bool found READ GetFound NOTIFY foundChanged)
	Q_PROPERTY(QString noteName READ GetNoteName NOTIFY noteNameChanged)

	private:
	QAudioRecorder *recorder;
	QAudioEncoderSettings settings;
	QAudioProbe *probe;

	LinearFilter<int16_t> *high_filter;
	ZeroCross<int16_t> *cross;
	Scale *scale;
	static const char *filename_record;
	std::ofstream file_record;

	bool running, found;
	double freq, deviation;
	int note, octave, nb_sample_running;
	int note_found, octave_found, count_found, count_not_found;
	int nb_deviation, deviation_start;
	double deviation_sum;

	static const int rate = 16000;
	static const int defaultNbFrame = 1024;
	static const int defaultFreqMin = 50;
	static const int defaultFreqMax = 2000;
	static const int nbSamplePreventRunning = rate * 40; // 40 seconds
	/// number of analyses to confirm a note
	static const int nbConfirm = 3;
	/// number of analyses to drop a note
	static const int nbDefect = 20;
	/// number of deviation values for average
	static const int nbDeviationValues = 8;

	double deviation_values[nbDeviationValues];

	inline void ComputeFrame(int16_t v);
	void SetFound(int note, int octave, double deviation);
	void SetNotFound();
	void ResetDeviation();
	void UpdateDeviation(double d);

	private slots:
	void AudioCb(const QAudioBuffer &buffer);

	public:
	Tuner();
	~Tuner();

	void Start();
	void Stop();

	void AudioAnalyse(const int16_t *buffer, int size);

	bool GetRunning();
	void SetRunning(bool r);
	double GetFreq();
	int GetNote();
	int GetOctave();
	double GetDeviation();
	bool GetFound();
	const char* GetNoteName();

	/// analyse a file for debug
	static void analyse_file(const char *filename);
	/// write a file with raw audio
	static void set_record(const char *filename_record);

	signals:
	void runningChanged();
	void freqChanged();
	void noteChanged();
	void noteNameChanged();
	void octaveChanged();
	void deviationChanged();
	void foundChanged();
};
