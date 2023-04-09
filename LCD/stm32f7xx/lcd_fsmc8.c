/*
 * 8 bit paralell LCD GPIO driver for STM32F7
 * 5 controll pins (CS, RS, WR, RD, RST) + 8 data pins + backlight pin
 */

/* 
 * Author: Roberto Benjami
 * version:  2020.05.28
 */

#include "main.h"
#include "lcd.h"
#include <syscfg.h>
#include <fsmc.h>




#define DELAY_WE    asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP"); \
                    asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");



#define DELAY_WE1    asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
                    

//LCD pins Defines
gpiopin LCD_NOE = {GPIOD, GPIO4}; //AF12
gpiopin LCD_NWE = {GPIOD, GPIO5}; //AF12
gpiopin LCD_NE = {GPIOD, GPIO7}; //AF9  ////////////////////////////////////////////////////////////
gpiopin LCD_RESET = {GPIOG, GPIO8}; 
gpiopin LCD_A0 = {GPIOF, GPIO0}; //0 : CMD
gpiopin LCD_D0 = {GPIOD, GPIO14}; //AF12
gpiopin LCD_D1 = {GPIOD, GPIO15}; //AF12
gpiopin LCD_D2 = {GPIOD, GPIO0}; //AF12
gpiopin LCD_D3 = {GPIOD, GPIO1}; //AF12
gpiopin LCD_D4 = {GPIOE, GPIO7}; //AF12
gpiopin LCD_D5 = {GPIOE, GPIO8}; //AF12
gpiopin LCD_D6 = {GPIOE, GPIO9}; //AF12
gpiopin LCD_D7 = {GPIOE, GPIO10}; //AF12

gpiopin LCD_A24 = {GPIOG, GPIO13}; //AF12

gpiopin LCD_DrvE = {GPIOG, GPIO1}; //AF12
extern Display DisplayLCD;


uint32_t __rbit(uint32_t inp32)
{
    
    uint32_t out = 0;
    asm("RBIT %0, %1" : "=r" (out) : "r" (inp32));
    return out;
    
   return inp32;
}


void LCD_IO_Bl_OnOff(uint8_t Bl)
{
    if(Bl==0){
        gpio_clear(LCD_DrvE.port, LCD_DrvE.pin);
        DisplayLCD.isON=false;
    }
    else{
        gpio_set(LCD_DrvE.port, LCD_DrvE.pin);
        DisplayLCD.isON=true;
    }
}

//-----------------------------------------------------------------------------
void LCD_IO_Init(void)
{
    SYSCFG_MEMRM |= 1<<10; //swap sram and sdram mapping to let the cpu know sram is an external memory
                            //Otherwise, the CPU tries to optimize the data transfer which fucks up the timings
    rcc_periph_clock_enable(RCC_FMC);

    //gpio_mode_setup(LCD_A24.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_A24.pin);
    //gpio_set_af(LCD_A24.port, GPIO_AF12, LCD_A24.pin);

    gpio_mode_setup(LCD_NWE.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_NWE.pin);
    gpio_set_af(LCD_NWE.port, GPIO_AF12, LCD_NWE.pin);

    gpio_mode_setup(LCD_NE.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LCD_NE.pin);
    //gpio_set_af(LCD_NE.port, GPIO_AF9, LCD_NE.pin);
    gpio_set_output_options(LCD_NE.port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_NE.pin);
    gpio_set(LCD_NE.port, LCD_NE.pin);

    gpio_mode_setup(LCD_NOE.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_NOE.pin);
    gpio_set_af(LCD_NOE.port, GPIO_AF12, LCD_NOE.pin);

    gpio_mode_setup(LCD_RESET.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LCD_RESET.pin);
    gpio_set_output_options(LCD_RESET.port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LCD_RESET.pin);

    gpio_mode_setup(LCD_DrvE.port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_DrvE.pin);
    gpio_set_output_options(LCD_DrvE.port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LCD_DrvE.pin);

    gpio_mode_setup(LCD_A0.port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_A0.pin);
    gpio_set_output_options(LCD_A0.port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_A0.pin);

    gpio_mode_setup(LCD_D0.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D0.pin);
    gpio_set_af(LCD_D0.port, GPIO_AF12, LCD_D0.pin);

    gpio_mode_setup(LCD_D1.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D1.pin);
    gpio_set_af(LCD_D1.port, GPIO_AF12, LCD_D1.pin);

    gpio_mode_setup(LCD_D2.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D2.pin);
    gpio_set_af(LCD_D2.port, GPIO_AF12, LCD_D2.pin);

    gpio_mode_setup(LCD_D3.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D3.pin);
    gpio_set_af(LCD_D3.port, GPIO_AF12, LCD_D3.pin);

    gpio_mode_setup(LCD_D4.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D4.pin);
    gpio_set_af(LCD_D4.port, GPIO_AF12, LCD_D4.pin);

    gpio_mode_setup(LCD_D5.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D5.pin);
    gpio_set_af(LCD_D5.port, GPIO_AF12, LCD_D5.pin);

    gpio_mode_setup(LCD_D6.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D6.pin);
    gpio_set_af(LCD_D6.port, GPIO_AF12, LCD_D6.pin);

    gpio_mode_setup(LCD_D7.port, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_D7.pin);
    gpio_set_af(LCD_D7.port, GPIO_AF12, LCD_D7.pin);

    
    
    FSMC_BCR1 |= (1<<14); //EXTMOD
    FSMC_BCR1 &= ~(0x3<<4); //MWID
    FSMC_BCR1 &= ~(0x3<<2); //MTYP 
    FSMC_BCR1 |= 1; //MBKEN
    
    FSMC_BTR1 = 0x001F0411;

    FSMC_BWTR1= 0x0FF10811;
    

    gpio_clear(LCD_RESET.port, LCD_RESET.pin);
    delay_ms(1000);
    gpio_set(LCD_RESET.port, LCD_RESET.pin);
    delay_ms(1000);
    gpio_set(LCD_DrvE.port, LCD_DrvE.pin);
    gpio_set(LCD_A0.port, LCD_A0.pin);
    
    gpio_clear(LCD_NE.port, LCD_NE.pin);
    delay_ms(1000);

    /*

    gpio_clear(LCD_RESET.port, LCD_RESET.pin);
    delay_ms(1000);
    gpio_set(LCD_RESET.port, LCD_RESET.pin);
    gpio_set(LCD_DrvE.port, LCD_DrvE.pin);
    gpio_set(LCD_A0.port, LCD_A0.pin);
    gpio_clear(LCD_NE.port, LCD_NE.pin);

    delay_ms(1);

    FSMC_BCR1 |= (1<<14); //EXTMOD
    FSMC_BCR1 &= ~(0x3<<4); //MWID
    FSMC_BCR1 &= ~(0x3<<2); //MTYP
    FSMC_BCR1 |= 1; //MBKEN

    FSMC_BTR1 = 0x00100411;

    FSMC_BWTR1= 0x0FF10811;
*/

}


//-----------------------------------------------------------------------------
void LCD_IO_WriteCmd8MultipleData8(uint8_t cmd, uint8_t* data, uint32_t ndata){
/*
        uint32_t rbit_v = __rbit(cmd);
        rbit_v = rbit_v>>24;
        cmd = (uint8_t) rbit_v;
*/
        gpio_clear(LCD_A0.port, LCD_A0.pin);
        
        MMIO8(SRAM_SECTOR1) = cmd;
        DELAY_WE

        gpio_set(LCD_A0.port, LCD_A0.pin);
        size_t i = 0;
        for (; i < ndata; i++)
        {
            /*
            rbit_v = __rbit(data[i]);
            rbit_v = rbit_v>>24;
            data[i] = (uint8_t) rbit_v;
*/
            MMIO8(SRAM_SECTOR1+1+i) = data[i];
            
            //LCD->lcd_ram = data[i];
        }
        //delay_ms(1);
    

}


//-----------------------------------------------------------------------------
void LCD_IO_WriteCmd8(uint8_t cmd){
/*
        uint32_t rbit_v = __rbit(cmd);
        rbit_v = rbit_v>>24;
        cmd = (uint8_t) rbit_v;
*/
        gpio_clear(LCD_A0.port, LCD_A0.pin);
        MMIO8(SRAM_SECTOR1) = cmd;

        DELAY_WE
        gpio_set(LCD_A0.port, LCD_A0.pin);
    
}

//-----------------------------------------------------------------------------
void LCD_IO_WriteData8(uint8_t data){
 /*   
        uint32_t rbit_v = __rbit(data);
        rbit_v = rbit_v>>24;
        data = (uint8_t) rbit_v;
*/
        MMIO8(SRAM_SECTOR1) = data;
    
}

void LCD_IO_WriteData16(uint16_t data){
 /*   
        uint32_t rbit_v = __rbit(data);
        rbit_v = rbit_v>>16;
        data = (uint16_t) rbit_v;
*/
        MMIO8(SRAM_SECTOR1) = data>>8;
        
        MMIO8(SRAM_SECTOR1+1) = data;
        
}

//-----------------------------------------------------------------------------
void LCD_IO_WriteCmd8DataFill16(uint8_t cmd, uint16_t data, uint32_t Size){
    /*
        uint32_t rbit_v = __rbit(cmd);
        rbit_v = rbit_v>>24;
        cmd = (uint8_t) rbit_v;
        rbit_v=0;
        */

        gpio_clear(LCD_A0.port, LCD_A0.pin);
        MMIO8(SRAM_SECTOR1) = cmd;
        //LCD->lcd_reg = cmd;
        DELAY_WE
        gpio_set(LCD_A0.port, LCD_A0.pin);

        size_t i = 0;
        for (; i < Size; i++)
        {

            MMIO8(SRAM_SECTOR1+1+2*i) = data;
            MMIO8(SRAM_SECTOR1+2+2*i) = data>>8;

            //LCD_IO_WriteData8((data) >> 8);
            //LCD_IO_WriteData8(data);
            
            
            //LCD->lcd_ram = data[i];
        }
        //delay_ms(1);
        
    
}


//-----------------------------------------------------------------------------
void LCD_IO_WriteCmd8MultipleData16(uint8_t cmd, uint16_t *data, uint32_t Size){
/*    
        uint32_t rbit_v = __rbit(cmd);
        rbit_v = rbit_v>>24;
        cmd = (uint8_t) rbit_v;
*/
        gpio_clear(LCD_A0.port, LCD_A0.pin);
        MMIO8(SRAM_SECTOR1) = cmd;
        //LCD->lcd_reg = cmd;

        DELAY_WE
        gpio_set(LCD_A0.port, LCD_A0.pin);
        size_t i = 0;
        for (; i < Size; i++)
        {
/*
            rbit_v = __rbit(data[i]);
            rbit_v = rbit_v>>16;
            data[i] = (uint16_t) rbit_v;
*/
            MMIO8(SRAM_SECTOR1+1+2*i) = data[i];
            MMIO8(SRAM_SECTOR1+2+2*i) = data[i]>>8;
            //LCD->lcd_ram = data[i];
        }
        //delay_ms(1);
    
    
}

//-----------------------------------------------------------------------------
void LCD_IO_ReadCmd8MultipleData8(uint8_t cmd, uint8_t *data, uint32_t Size, uint32_t DummySize){
    /*
        uint32_t rbit_v = __rbit(cmd);
        rbit_v = rbit_v>>24;
        cmd = (uint8_t) rbit_v;
*/
        gpio_clear(LCD_A0.port, LCD_A0.pin);
        MMIO8(SRAM_SECTOR1) = cmd;

        DELAY_WE
        gpio_set(LCD_A0.port, LCD_A0.pin);
        
        size_t i = 0;
        for (; i < DummySize; i++)
        {
            data[i]=MMIO8(SRAM_SECTOR1);
        }

        for (; i < Size+DummySize; i++)
        {
            data[i]=MMIO8(SRAM_SECTOR1+1+i);
/*
            rbit_v = __rbit(data[i]);
            rbit_v = rbit_v>>24;
            data[i] = (uint8_t) rbit_v;
            */
        }
     
}

//-----------------------------------------------------------------------------
void LCD_IO_ReadCmd8MultipleData16(uint8_t cmd, uint16_t *data, uint32_t Size, uint32_t DummySize){
    //not used
}

void     LCD_IO_ReadCmd8MultipleData24to16(uint8_t cmd, uint16_t *data, uint32_t Size, uint32_t DummySize){
    //does nothing
}

uint8_t data_test[11]={0b01010101, 0b10101010, 0b01010101, 0b10101010, 0b01010101, 0b10101010, 0b01010101, 0b10101010, 0b01010101, 0b10101010, 0b11111111};

void LCD_IO_test(){
    LCD_IO_WriteCmd8MultipleData8(0b10101010, data_test, 11);
}