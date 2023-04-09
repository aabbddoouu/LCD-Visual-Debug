#pragma once
#include <main.h>

#define ADC_CURRENT         ADC3
#define ADC_CURRENT_CHANNEL ADC_CHANNEL9
#define ADC_V0_CHANNEL      ADC_CHANNEL8
#define ADC_FILTER_LENGHT   10

void setup_current_M();

typedef struct
{
    uint32_t Current;
    FLOAT   Current_f;
    FLOAT   V0_f;
    bool    Update_C;
    bool    first_conv; //replace with uint8_t if > 2
    FLOAT   FilterAVG[ADC_FILTER_LENGHT];
    FLOAT   V0AVG[ADC_FILTER_LENGHT];
    uint8_t Filter_index

}ADC_H;
