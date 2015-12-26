#include <QAudioRecorder>
#include <QAudioProbe>

#include "audio/LinearFilter.hpp"
#include "audio/ZeroCross.hpp"

class Tuner : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool running READ GetRunning WRITE SetRunning NOTIFY runningChanged)
	Q_PROPERTY(double freq READ GetFreq NOTIFY freqChanged)

	private:
	QAudioRecorder *recorder;
	QAudioEncoderSettings settings;
	QAudioProbe *probe;

	LinearFilter<int16_t> *high_filter;
	ZeroCross<int16_t> *cross;

	bool running;
	double freq;

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

	bool GetRunning();
	void SetRunning(bool r);
	double GetFreq();

	signals:
	void runningChanged();
	void freqChanged();
};
