#pragma once 

#include<main.h>


typedef struct lcd_fmc_address_st {
    
    uint8_t lcd_reg;
    uint8_t lcd_ram;
} lcd_fmc_address_t;

void setup_lcd();
void Read_Reg(uint8_t cmd, uint8_t* data, uint32_t ndata);
void Write_Reg(uint8_t cmd, uint8_t* data, uint32_t ndata);


