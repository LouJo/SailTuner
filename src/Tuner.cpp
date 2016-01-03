#include <iostream>
#include <stdint.h>

#include <QCoreApplication>
#include <QUrl>
#include <QDBusConnection>
#include <QDBusInterface>

#include "Tuner.hpp"

using namespace std;

// high 10hz / 16k
static double a10[] = { 1        , -2.99214602,  2.98432286, -0.99217678 };
static double b10[] = { 0.99608071, -2.98824212,  2.98824212, -0.99608071 };

// function to prevent screen blank

static void blank_prevent(bool prevent)
{
	cerr << __func__ << endl;
	QDBusConnection system = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
	QDBusInterface interface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", system);

	if (prevent) {
		interface.call(QLatin1String("req_display_blanking_pause"));
	} else {
		interface.call(QLatin1String("req_display_cancel_blanking_pause"));
	}
}

Tuner::Tuner()
{
	running = false;
	freq = deviation = 0;
	note = octave = 0; 
	found = false;
	count_found = count_not_found = 0;

	high_filter = new LinearFilter<int16_t>(3, a10, b10);

	ZeroCross<int16_t>::Config cross_config({rate, defaultNbFrame, defaultFreqMin, defaultFreqMax});
	cross = new ZeroCross<int16_t>(cross_config);

	scale = new Scale();
	scale->ConstructEqualTemperament();

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
	delete scale;
}

void Tuner::Start()
{
	cerr << __func__ << endl;
	count_found = count_not_found = 0;
	nb_sample_running = 0;
	note_found = octave_found = -1;
	ResetDeviation();
	blank_prevent(true);
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
	blank_prevent(false);
}

void Tuner::ComputeFrame(int16_t v)
{
	v = (*high_filter)(v);
	(*cross)(v);
}

void Tuner::AudioCb(const QAudioBuffer &buffer)
{
	const int16_t *ptr = buffer.constData<int16_t>();
	const int nbFrame = buffer.sampleCount();
	AudioAnalyse(ptr, nbFrame);
}

void Tuner::ResetDeviation()
{
	// reset deviation values
	nb_deviation = 0;
	deviation_start = 0;
	deviation_sum = 0;
}

void Tuner::UpdateDeviation(double d)
{
	if (nb_deviation == nbDeviationValues) {
		deviation_sum -= deviation_values[deviation_start];
		deviation_start = (deviation_start + 1) % nbDeviationValues;
		nb_deviation--;
	}
	deviation_values[(deviation_start + nb_deviation) % nbDeviationValues] = d;
	nb_deviation++;
	deviation_sum += d;
}

void Tuner::SetFound(int n, int o, double d)
{
	if (n != note_found || o != octave_found) {
		note_found = n;
		octave_found = o;
		count_found = 0;
		SetNotFound();

		ResetDeviation();
		UpdateDeviation(d);
	}
	else if (count_found++ >= nbConfirm) {
		count_not_found = 0;
		if (!found) {
			found = true;
			foundChanged();
		}
		if (note != n) {
			note = n;
			noteChanged();
		}
		if (octave != o) {
			octave = o;
			octaveChanged();
		}
		UpdateDeviation(d);
		deviation = deviation_sum / nb_deviation;
		deviationChanged();
	}
	else {
		UpdateDeviation(d);
	}
}

void Tuner::SetNotFound()
{
	if (count_not_found++ >= nbDefect) {
		count_found = 0;
		if (found) {
			found = false;
			ResetDeviation();
			foundChanged();
		}
	}
}

void Tuner::AudioAnalyse(const int16_t *ptr, int nb_frame)
{
	nb_sample_running += nb_frame;

	while (nb_frame--) ComputeFrame(*ptr++);

	if (freq != cross->Freq()) {
		freq = cross->Freq();
		freqChanged();

		if (freq) {
			int n, o;
			double d;
			n = scale->FindNote(freq, o, d);
			SetFound(n, o, d);
		}
		else { // no freq
			SetNotFound();
		}
	}

	if (nb_sample_running >= nbSamplePreventRunning && running) {
		nb_sample_running = 0;
		blank_prevent(true);
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

int Tuner::GetNote()
{
	return note;
}

double Tuner::GetDeviation()
{
	return deviation;
}

int Tuner::GetOctave()
{
	return octave;
}

const char* Tuner::GetNoteName()
{
	return scale->NoteName(note);
}

bool Tuner::GetFound()
{
	return found;
}
