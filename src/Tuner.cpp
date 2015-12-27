#include <iostream>
#include <stdint.h>

#include <QCoreApplication>
#include <QUrl>

#include "Tuner.hpp"

using namespace std;

// high 10hz / 16k
static double a10[] = { 1        , -2.99214602,  2.98432286, -0.99217678 };
static double b10[] = { 0.99608071, -2.98824212,  2.98824212, -0.99608071 };

Tuner::Tuner()
{
	running = false;

	high_filter = new LinearFilter<int16_t>(3, a10, b10);

	ZeroCross<int16_t>::Config cross_config({rate, defaultNbFrame, defaultFreqMin, defaultFreqMax});
	cross = new ZeroCross<int16_t>(cross_config);

	settings.setCodec("audio/PCM");
	settings.setChannelCount(1);
	settings.setSampleRate(rate);

	recorder = new QAudioRecorder(this);
	recorder->setAudioInput("pulseaudio:");
	recorder->setAudioSettings(settings);

	QUrl url = QString("/dev/null");
	recorder->setOutputLocation(url);

	probe = new QAudioProbe(this);
	connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(AudioCb(QAudioBuffer)));
	probe->setSource(recorder);
}

Tuner::~Tuner()
{
	delete high_filter;
	delete cross;
	delete recorder;
}

void Tuner::Start()
{
	cerr << __func__ << endl;
	high_filter->Clear();
	cross->Clear();
	recorder->record();
	running = true;
	runningChanged();
}

void Tuner::Stop()
{
	cerr << __func__ << endl;
	running = false;
	recorder->stop();
	runningChanged();
}

void Tuner::ComputeFrame(int16_t v)
{
	v = (*high_filter)(v);
	(*cross)(v);
}

void Tuner::AudioCb(const QAudioBuffer &buffer)
{
	const int16_t *ptr = buffer.constData<int16_t>();
	int nbFrame = buffer.sampleCount();

	while (nbFrame--) ComputeFrame(*ptr++);

	if (freq != cross->Freq()) {
		freq = cross->Freq();
		freqChanged();
	}
}

bool Tuner::GetRunning()
{
	return running;
}

void Tuner::SetRunning(bool r)
{
	if (running == r) return;

	if (r) Start();
	else Stop();
}

double Tuner::GetFreq()
{
	return freq;
}
