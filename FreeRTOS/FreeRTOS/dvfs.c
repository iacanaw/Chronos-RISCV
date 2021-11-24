#include "dvfs.h"

void API_setFreqIdle(){
    selectedFrequencyScale = FREQ_IDLE;
    return;
}

void API_setFreqScale(unsigned int scale){
    selectedFrequencyScale = scale;
    return;
}

unsigned int API_getFreqScale(){
    return HW_get_32bit_reg(FREQUENCY_SCALE);
}

void API_applyFreqScale(){
    if(selectedFrequencyScale != API_getFreqScale()){
        HW_set_32bit_reg(FREQUENCY_SCALE, selectedFrequencyScale);
    }
    return;
}

void API_freqStepUp(){
    if(selectedFrequencyScale == FREQ_IDLE){
        selectedFrequencyScale = FREQ_150MHz;
    }
    else if(selectedFrequencyScale==FREQ_150MHz){
        selectedFrequencyScale = FREQ_250MHz;
    }
    else if(selectedFrequencyScale==FREQ_250MHz){
        selectedFrequencyScale = FREQ_500MHz;
    } 
    else if(selectedFrequencyScale==FREQ_500MHz){
        selectedFrequencyScale = FREQ_1GHz;
    }
    else if(selectedFrequencyScale==FREQ_1GHz){
        selectedFrequencyScale = FREQ_1GHz;
    }
    else {
        if(selectedFrequencyScale > FREQ_1GHz){
            selectedFrequencyScale = FREQ_1GHz;
        }
        else if(selectedFrequencyScale > FREQ_500MHz){
            selectedFrequencyScale = FREQ_1GHz;
        }
        else if(selectedFrequencyScale > FREQ_250MHz){
            selectedFrequencyScale = FREQ_500MHz;
        }
        else if(selectedFrequencyScale > FREQ_150MHz){
            selectedFrequencyScale = FREQ_250MHz;
        }
        else if(selectedFrequencyScale > FREQ_IDLE){
            selectedFrequencyScale = FREQ_150MHz;
        }
        else{
            selectedFrequencyScale = FREQ_IDLE;
        }
    }
    return;
}

void API_freqStepDown(){
    if(selectedFrequencyScale == FREQ_IDLE){
        selectedFrequencyScale = FREQ_IDLE;
    }
    else if(selectedFrequencyScale == FREQ_150MHz){
        selectedFrequencyScale = FREQ_IDLE;
    }
    else if(selectedFrequencyScale == FREQ_250MHz){
        selectedFrequencyScale = FREQ_150MHz;
    }
    else if(selectedFrequencyScale == FREQ_500MHz){
        selectedFrequencyScale = FREQ_250MHz;
    }
    else if(selectedFrequencyScale == FREQ_1GHz){
        selectedFrequencyScale = FREQ_500MHz;
    }
    else{
        if(selectedFrequencyScale < FREQ_IDLE){
            selectedFrequencyScale = FREQ_IDLE;
        }
        else if(selectedFrequencyScale < FREQ_150MHz){
            selectedFrequencyScale = FREQ_IDLE;
        }
        else if(selectedFrequencyScale < FREQ_250MHz){
            selectedFrequencyScale = FREQ_150MHz;
        }
        else if(selectedFrequencyScale < FREQ_500MHz){
            selectedFrequencyScale = FREQ_250MHz;
        }
        else if(selectedFrequencyScale < FREQ_1GHz){
            selectedFrequencyScale = FREQ_500MHz;
        }
        else{
            selectedFrequencyScale = FREQ_1GHz;
        }
    }
    return;
}
