#include <main.h>
///////////////////////////////////////////
/// Psectro General Stuff
//////////////////////////////////////////

#define SPECTRO_UPDATE  100 //ms

#define UART_TIMEOUT_SETUP  1000U
#define UART_TIMEOUT_POST   80U

#define SPECTRO_IDLE    0
#define SPECTRO_RELOAD  1
#define SPECTRO_BUSY    2
#define SPECTRO_CONFIG  8
#define SPECTRO_START_E 9
#define SPECTRO_GET_S   10
#define SPECTRO_ACQUIRE 11
#define SPECTRO_PROCESS 12
#define SPECTRO_DAC     13

#define SPECTRO_WAITING 20
#define SPECTRO_CALIB   30

#define SPECTRO_UO_FLOW 50
#define SPECTRO_SATUR   51

#define SPECTRO_MENU    98
#define SPECTRO_DEFAULT 99



#define CALIB_PERIOD    36000000







///////////////////////////////////////////
/// DAC SPI interface
//////////////////////////////////////////

#define SPI_DAC         SPI1
#define RCC_SPI_DAC     RCC_SPI1
#define SPI_DAC_GPIO    GPIOA
#define SPI_DAC_SCLK    GPIO5
#define SPI_DAC_MOSI    GPIO7
#define SPI_DAC_MISO    GPIO6
#define SPI_DAC_AF      GPIO_AF5
#define DAC_CS_PORT     GPIOA
#define DAC_CS          GPIO4

#define DAC_FAULT_PORT  GPIOB
#define DAC_FAULT1      GPIO5
#define DAC_FAULT2      GPIO4









///////////////////////////////////////////
/// Spectro UART interface
//////////////////////////////////////////

#define UART_SPECTRO        UART5
#define RCC_UART_SPECTRO    RCC_UART5 //UART7 old
#define UART_SPECTRO_GPIO   GPIOB
#define UART_SPECTRO_AF     GPIO_AF8

#define SPECTRO_RESET_GPIO  GPIOB
#define SPECTRO_RESET_PIN   GPIO11

#define RX_PIN              GPIO12 //GPIO3 old
#define TX_PIN              GPIO13 //GPIO4 old


#define TX7_LEN             1028
#define RX7_LEN             1025
#define TX7_DATA_INDEX      3

#define Spectra_Start_index 13

///////////////////////////////////////////
/// NioLink CMDs
//////////////////////////////////////////
// Interface layer CMDs
#define WRITE_CMD           0xF102
#define READ_CMD            0xF002
#define GETSTATUS_CMD       0xF000
#define GETPROTOCOL_CMD     0xF004

// Interface layer Ret Codes
#define INT_OK              0x0000
#define INT_CMD_CODE        0x0001
#define INT_INV_PARAM       0x0002
#define INT_INV_OP          0x0004
#define INT_NOT_SUPP        0x0005
#define INT_COM_ERR         0x0007
#define INT_INTERNAL_ERR    0x0008
#define INT_NO_DATA_AVA     0x000A
#define INT_WAITING_REPLY   0x0010


// Message layer CMDs
// Device CMD
#define RESET_MSG           0x0000
#define STOP_COM_MSG        0x0001
#define RESET_PARAM_MSG     0x0003
#define START_EXPOSURE_MSG  0x0004
#define CANCEL_EXPOSURE_MSG 0x0005
// Device Property
#define GetDeviceID_MSG     0x2000
#define GetSerialNo_MSG     0x2001
#define GetMan_MSG          0x2002
#define GetModelN_MSG       0x2003
#define GetHWversion_MSG    0x2004
#define GetSWversion_MSG    0x2005
#define GetMaxValue_MSG     0x2006
#define GetPixelCout_MSG    0x2007
// Device Values
#define GetStatus_MSG       0x3000
#define GetSensorTemp_MSG   0x3001

// Device Bulk Data
#define GetSpectra_MSG      0x4000
#define GetWavelengths_MSG  0x4001
#define GetCalibrationData  0x4002
#define GetUserData         0x4003
#define SetCalibrationData  0x4102
#define SetUserData         0x4103 

// Device Parameters
#define SetExposureTime_MSG 0x1100  
#define SetAvgValue_MSG     0x1101
#define SetProcessing_MSG   0x1102    
#define TurnOffLeds_MSG     0x1108

// Processing Steps
#define STEPS_OFF            (0U)
#define STEP_ADJ_OFF         (1<<0)
#define STEP_CORR_NLIN       (1<<1)
#define STEP_BAD_PIXELS      (1<<2)
#define STEP_SUB_DARK        (1<<3)
#define STEP_PIXEL_SPIKES    (1<<4) 
#define STEP_STRAYLIGHT      (1<<5)  
#define STEP_NORM_IT         (1<<6)  
#define STEP_SPEC_SENS       (1<<7)  
#define STEP_PRNU            (1<<8)
#define STEP_ADD_FILTER      (1<<9)
#define STEP_SCALE_16BITS    (1<<10)  

// Calibration Data Indexes
#define INDEX_LOWGain       11+TX7_DATA_INDEX
#define INDEX_HIGHGain      13+TX7_DATA_INDEX
#define INDEX_IT_COUNT      73+TX7_DATA_INDEX
#define INDEX_IT_VAL        74+TX7_DATA_INDEX
#define INDEX_NLIN_COUNT    29+TX7_DATA_INDEX
#define INDEX_NLIN_VAL      30+TX7_DATA_INDEX
#define INDEX_OFFSET_FIRST  106+TX7_DATA_INDEX
#define INDEX_OFFSET_LAST   233+TX7_DATA_INDEX

// Message layer Ret Codes
#define MSG_OK              0x0000
#define MSG_UNKOWN_CMD      0x0001
#define MSG_INV_PARAM       0x0002
#define MSG_MISS_PARAM      0x0003
#define MSG_INV_OP          0x0004
#define MSG_FUNC_SUPP       0x0005
#define MSG_PASSCODE        0x0006
#define MSG_COM_ERR         0x0007
#define MSG_INTERNAL_ERR    0x0008
#define MSG_UNKOWN_BOOTL    0x0009

// Message layer types
#define MSG_INT32           0x0000
#define MSG_UINT32          0x0001
#define MSG_FLOAT           0x0002
#define MSG_VERSION         0x0003
#define MSG_STRING          0x0004
#define MSG_BOOL            0x0005
#define MSG_HEX             0x0006
#define MSG_DATE            0x0007

#define ERR_length_MATCH    0xFFFF

#define FP_DEC_RES          1.f/16



void uart_SendCommand (uint16_t command, uint16_t nargs, ...);
void uart_SendCommand_dma (uint16_t command, uint16_t nargs, bool Tx_preloaded,  ...);
void start_uart_rx_reception(uint32_t n_bytes);
void start_uart_tx_transfer(uint32_t n_words);
void setup_Spectro_usart();
void init_uart_dma();
uint16_t uart_ReadResp();
uint32_t uart_ReadResp32();
void send_receiveSpectro (uint16_t command, uint16_t nargs, bool Tx_preloaded, ...);
uint32_t print_Spectro_Data(uint16_t command_sent, uint16_t p_length);