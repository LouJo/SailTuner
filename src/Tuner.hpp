#include <QAudioRecorder>
#include <QAudioProbe>

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
	Q_PROPERTY(QString noteName READ GetNoteName NOTIFY noteNameChanged)

	private:
	QAudioRecorder *recorder;
	QAudioEncoderSettings settings;
	QAudioProbe *probe;

	LinearFilter<int16_t> *high_filter;
	ZeroCross<int16_t> *cross;
	Scale *scale;

	bool running;
	double freq, deviation;
	int note, octave;

	static const int rate = 16000;
	static const int defaultNbFrame = 1024;
	static const int defaultFreqMin = 50;
	static const int defaultFreqMax = 2000;

	inline void ComputeFrame(int16_t v);

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
	const char* GetNoteName();

	signals:
	void runningChanged();
	void freqChanged();
	void noteChanged();
	void noteNameChanged();
	void octaveChanged();
	void deviationChanged();
};
