#include "dvfs.h"

void API_setFreqIdle(){
    prints("API_setFreqIdle\n ");
    *frequencyScale = FREQ_IDLE;
    return;
}

void API_setFreqScale(unsigned int scale){
    *frequencyScale = scale;
    return;
}

unsigned int API_getFreqScale(){
    return *frequencyScale;
}

void API_freqStepUp(){
    if(*frequencyScale == FREQ_IDLE){
        *frequencyScale = FREQ_100MHz;
    }
    else if(*frequencyScale==FREQ_100MHz){
        *frequencyScale = FREQ_250MHz;
    }
    else if(*frequencyScale==FREQ_250MHz){
        *frequencyScale = FREQ_500MHz;
    } 
    else if(*frequencyScale==FREQ_500MHz){
        *frequencyScale = FREQ_1GHz;
    }
    else if(*frequencyScale==FREQ_1GHz){
        *frequencyScale = FREQ_1GHz;
    }
    else {
        if(*frequencyScale > FREQ_1GHz){
            *frequencyScale = FREQ_1GHz;
        }
        else if(*frequencyScale > FREQ_500MHz){
            *frequencyScale = FREQ_1GHz;
        }
        else if(*frequencyScale > FREQ_250MHz){
            *frequencyScale = FREQ_500MHz;
        }
        else if(*frequencyScale > FREQ_100MHz){
            *frequencyScale = FREQ_250MHz;
        }
        else if(*frequencyScale > FREQ_IDLE){
            *frequencyScale = FREQ_100MHz;
        }
        else{
            *frequencyScale = FREQ_IDLE;
        }
    }
    return;
}

void API_freqStepDown(){
    if(*frequencyScale == FREQ_IDLE){
        *frequencyScale = FREQ_IDLE;
    }
    else if(*frequencyScale == FREQ_100MHz){
        *frequencyScale = FREQ_IDLE;
    }
    else if(*frequencyScale == FREQ_250MHz){
        *frequencyScale = FREQ_100MHz;
    }
    else if(*frequencyScale == FREQ_500MHz){
        *frequencyScale = FREQ_250MHz;
    }
    else if(*frequencyScale == FREQ_1GHz){
        *frequencyScale = FREQ_500MHz;
    }
    else{
        if(*frequencyScale < FREQ_IDLE){
            *frequencyScale = FREQ_IDLE;
        }
        else if(*frequencyScale < FREQ_100MHz){
            *frequencyScale = FREQ_IDLE;
        }
        else if(*frequencyScale < FREQ_250MHz){
            *frequencyScale = FREQ_100MHz;
        }
        else if(*frequencyScale < FREQ_500MHz){
            *frequencyScale = FREQ_250MHz;
        }
        else if(*frequencyScale < FREQ_1GHz){
            *frequencyScale = FREQ_500MHz;
        }
        else{
            *frequencyScale = FREQ_1GHz;
        }
    }
    return;
}
