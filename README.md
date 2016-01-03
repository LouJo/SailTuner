# SailTuner

SailTuner is a chromatic multi-temperament instrument tuner for [Sailfish OS](https://sailfishos.org/).

It gets audio data from microphone, find the fundamental frequency, and then display the note, the octave and the deviation according to note reference and temperament.

### Pitch detection algorithm

The algorithm for fundamental frequency detection:

- high-pass filter (10Hz)
- zero-crossing detection with linear interpolation
- pattern recognition from regions, with time based standart deviation
- octave filtering: drop a frequency if already a higher octave for same note in some conditions
- post filter: wait few analyses to confirm or defect a note, and make a turning average for note deviation

The audio input rate is 16KHz ; audio frames are analysed every 1024 samples (64ms) and a note is confirmed after 4 chunks (256ms).

The note is found compared to an array of frequencies modulo one octave. The comparison array is built from a temperament and a 'A4' reference, default to 440Hz and equal temperament.

======

### Copyright

*Licence: GPLv3*  
*Author: Louis-Joseph Fournier*  
*First release date: 2016-??

https://github.com/LouJo/SailTuner
