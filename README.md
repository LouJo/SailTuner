# SailTuner

SailTuner is a chromatic multi-temperament instrument tuner for [Sailfish OS](https://sailfishos.org/).

It gets audio data from microphone, find the fundamental frequency, and then display the note, the octave and the deviation according to note reference and temperament.

It can also play a note, choosed with a scale flickable by user.

![](https://raw.githubusercontent.com/LouJo/SailTuner/master/images/screenshots/desktop-landscape.png)

### Pitch detection algorithm

The algorithm for fundamental frequency detection:

- high-pass filter (10Hz)
- zero-crossing detection with linear interpolation
- pattern recognition from regions, with time based standard deviation
- octave filtering: drop a frequency if already a higher octave for same note in some conditions
- post filter: wait few analyses to confirm or defect a note, and make a turning average for note deviation

The audio input rate is 16KHz ; audio frames are analysed every 2048 samples (128ms) and a note is confirmed after 3 chunks (384ms).

The note is found compared to an array of frequencies modulo one octave. The comparison array is built from a temperament and a 'A4' reference, default to 440Hz and equal temperament.

### Temperaments

The temperaments are stored in .csv files. At beginning, the programm scans every .csv file in the data/ dir and makes temperament list.

Every line in a file is a temperament: the first column is the temperament name, and columns 2 to 13 are the notes frequencies from C-4 to B-4

### Note player

A frequency player module makes the signal at given frequency. There are 3 waveforms in the code:

- Sinus
- Triangle (ie pair harmonics)
- Sinus + harmonics (1/n x f(n))

Since the speaker of Jolla phone have very low intensity for low frequencies, the third solution is used, and the intensity is higher for lower notes.

======

### Copyright

*Licence: GPLv3*  
*Author: Louis-Joseph Fournier*  
*First release date: 2016-01-19

https://github.com/LouJo/SailTuner
