#ifndef __DVFS_H__
#define __DVFS_H__

#define FREQUENCY_SCALE 0x8FFFFFF8u

//volatile unsigned int *frequencyScale = FREQUENCY_SCALE;
volatile unsigned int selectedFrequencyScale;

// 1GHz is the main frequency - other frequencies are relative to it
// FIXED POINT MATH: 1 means 0.1% ---- 10 means 1%
#define FREQ_IDLE   100      // 100MHz = 10% of 1GHz
#define FREQ_1GHz   1000     // 1GHz
#define FREQ_500MHz 500      // 500MHz = 50% of 1GHZ
#define FREQ_250MHz 250      // 250MHz = 25% of 1GHZ
#define FREQ_150MHz 150      // 100MHz = 10% of 1GHZ

void API_setFreqIdle();

void API_setFreqScale(unsigned int scale);

unsigned int API_getFreqScale();

void API_freqStepUp();

void API_freqStepDown();

void API_applyFreqScale();

#endif