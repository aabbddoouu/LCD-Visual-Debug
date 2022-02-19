# Atomm_src\AMS_AlgoDll_V310.c

- [x] MaxMin1D (lspec, LEN_PIX, &VMax, &i, &VMin, &i);
Finds the maximum and minimum values in the input array and their locations in the array. If there are multiple instances of the maximum and/or minimum values, MaxMin1D returns the location of the first occurrence. If all the elements in the input array are NaNs, MaxMin1D returns -1 for both the index of the maximum value and the index of the minimum value.

- [x] LPVOID
- [x] DWORD
- [x] HINSTANCE : if needed replace with int32_t ?
- [x] HANDLE : if needed replace with int32_t ?
- [x] __stdcall 
- [x] MessagePopup(str, str)
- [x] Local variables aka : ldarl lfd etc.
- [ ] if (VMax < gPmin) 

# Atomm_src\AMS_LibAlgo_V300.c & AMS_LiPlank_V300.c

- [x] ComplexNum
- [x] FILE
- [x] CxPolyRoots() : Calculates the roots of a real polynomial.
  Currently used for 2nd degree poly use delta -> RootsPoly2()
- [x] LinFit : Finds the slope and intercept values that best represent the linear fit of the data points (X, Y) using the least squares method.

    $\Large{Slope={\frac {\sum _{i=1}^{n}(x_{i}-{\bar {x}})(y_{i}-{\bar {y}})}{\sum _{i=1}^{n}(x_{i}-{\bar {x}})^{2}}}}$

    $\Large{Intercept = \bar{y}-Slope*\bar{x}}$

- [x] PolyEv1D :  $y_i=\sum_{n=1}^{N} coef_n*x_i^n$
- [x] All file management functions
- [x] in ReadFoncTransRef2005() : the file *NewRefTrans.txt* must have one coef per line per pixels
      
- [x] PresetAlgo()  remove it ?
- [x] Remap FmtOut to uart_printf

# Atomm_src\util_common_v100.c & Atomm_src\util_common_v100.h

only included for InitZoneMemoirePartageServNommee() used inside of VersionAlgo() which will not be used so this src file will not be compiled


# Testing code on existing data 

I need : 
- fdt (from readfonctrans ?)
- dark
- K trans
- TI ?
- LectSpectreRef : "*NewRef_Algo.txt"
- ReadFoncTransRef2005 : "*NewRef_Trans.txt"
And 
- raw data from spectro

**DONE.**

## Prediction function takes 763.000 cycles

- 683.000 	cycles of PredAlgo() 90% of the G-cost
- 49.000    cycles of AlgoPlanck()
- 31.000    of other functions/OPs

which represents 3.5ms @ 216MHz 

## PredAlgo()
### OPs at the start : 12.000 of which 
- 5.000 cycles of 
```
	for (ip=0; ip<UPIXEL; ip++) 
	{
		if (EvalSpec[ip] / gRefSpec[ip] <= 0.) 				return ERR_LOG0;
	}
```
why division ????? using multiplication reduces it to 32.000 cycles (better than checking the signs of each variable -> interesting)

Now since the div is used in the next section, we can compute it here to save time later.

- 6.000 cycles of 
```
	for (ip=0; ip<UPIXEL; ip++) 
	{
		for (ip=0; ip<UPIXEL; ip++) EvalSpec[ip] =  EvalSpec[ip] * GTiRef / (FLOAT) Mti; 
	}
```
reduced to 3.000 cycles by calculating the divition once.

- 1.000 for other stuff
 
### log_e(DIVISION) : 660.000 cycles  654.000 w/ change
Ouch ! Look like log is the bottleneck !
However we can use the fact the log_2() is easier to compute than log_e(). So instead of :

    log(Eval_Ref_div[ip]) 

we can instead use : (LOG2e is a constant housing log2f(e))

    log2f(Eval_Ref_div[ip]) / LOG2e

this reduces the cost from 654.000 to 50.000 !! without any precision trade-off (order of 10^-3 °C ==> nothing) 

### LinFit() : 5.500 cycles
MSE method is the lightest method so nothing we can do 

### RootPoly2() : Negligible


### PolyEv1D : Negligible

## AlgoPlank()
Just a wrapper for : 
### CalculTemp2() 
- uses pow(float, int) which can be replaced by successive VMULs. 
- uses log() which can be replaced by log2f()
It drops the cost from 50.000 to 4.500

So far, by making the above changes, I reduced the Total cost the prediction function down to  108.500 cycles.
Or 0.50ms @ 216 MHz

- 72.000  cycles of PredAlgo()    66.7% of  the G-cost
- 4.500   cycles of AlgoPlanck()   4.6% of  the G-cost
- 31.000  of other functions/OP   28.7% of  the G-cost




# Spectro Qneo 

## NioLink Protocol 

- [ ] Write SPIxfer32(), SPIxSend32(), SPIxRead32() functions 
- [ ] Add commands, return codes, etc. of the protocol
- [ ] Write Send_Command, Set_Param, Set_Processing, Get_BulkData ... functions
- [ ] Write Start_Spectro(), Get_Spectra(), Get_Wavelengths ... fucntions
- [ ] ... TBC


## Calibration Data
Calibration data has 	64 	pages with each page having 1024 values (Words)
page 0 and 32 are filled with values

User data has 		7 	pages with each page having 1024 values (Words)

when updating the dark spectra, page 0 changes from line 361

According to WAves the spectro is calibrated with 8 different exposure times 

GetCalibrationData 0

DATA 7  : 0x41c97150 - Fval : 25.180328 - Uval : 1103720784 

DATA 15  : 0x41e52400 - Fval : 28.642578 - Uval : 1105536000


GetCalibrationData 32

DATA 7  : 0x41e559f0 - Fval : 28.668915 - Uval : 1105549808 

DATA 15  : 0x41e52400 - Fval : 28.642578 - Uval : 1105536000 


### RaZ :

#### Dark Spec Calib 


## DAC xfer 
we need to adapt float values to the 16 bit DAC.
### 16 bit fixed point 
- 1 sign bit
- 11 bit integer part max 2047 (basically as 12 bit UINT)
- 4 bit decimal part : resolution = 0.0625
 

# Calibration Logic : when to trigger & Logic for 2 sensors
both must be triggred for a dark calibration 
if they cant be both triggred rely on one

# Params additional 
- Frequency of measurement without briquing the running code

# Hardware Fault management 

# How 2 use FMC (LCD) lule


























































































































