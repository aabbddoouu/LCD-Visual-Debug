#include <lcd.h>

extern uint8_t image_leo[153600];

Graph TGraph;
Display DisplayLCD;


void Display_Card(bool status){
	if(status){
		LCD_SetTextColor(DisplayLCD.OK_color);
	}
	else{
		LCD_SetTextColor(DisplayLCD.NOK_color);
	}

	LCD_FillRect(7,7,14,14);

	LCD_SetTextColor(DisplayLCD.Text_color);
}

void Display_Spectro(bool status){
	if(status){
		LCD_SetTextColor(DisplayLCD.OK_color);
	}
	else{
		LCD_SetTextColor(DisplayLCD.NOK_color);
	}

	LCD_FillRect(73,7,14,14);

	LCD_SetTextColor(DisplayLCD.Text_color);
}

void Display_DAC1(bool status){
	if(status){
		LCD_SetTextColor(DisplayLCD.OK_color);
	}
	else{
		LCD_SetTextColor(DisplayLCD.NOK_color);
	}

	LCD_FillRect(163,7,14,14);

	LCD_SetTextColor(DisplayLCD.Text_color);
}

void Display_DAC2(bool status){
	if(status){
		LCD_SetTextColor(DisplayLCD.OK_color);
	}
	else{
		LCD_SetTextColor(DisplayLCD.NOK_color);
	}

	LCD_FillRect(179,7,14,14);

	LCD_SetTextColor(DisplayLCD.Text_color);
}




void Display_Status(const char* str, uint16_t Color){
	if(!DisplayLCD.isON){
		return;
	}
	
	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(0, 27, ILI9341_LCD_PIXEL_WIDTH, 22);
	LCD_SetTextColor(Color);
	LCD_DisplayStringAt(0, 35, str, CENTER_MODE);
	LCD_SetTextColor(DisplayLCD.Text_color);

}

void init_display_struct(){
	DisplayLCD.Background_color=LCD_COLOR_WHITE;
    DisplayLCD.Font_Default=&Font12_7;
    DisplayLCD.Font_Temperature=&Font24_17;
    DisplayLCD.Graph_color=LCD_COLOR_MAGENTA;
    DisplayLCD.Text_color=LCD_COLOR_BLACK;
	DisplayLCD.OK_color=LCD_COLOR_GREEN;
    DisplayLCD.NOK_color=LCD_COLOR_RED;
	DisplayLCD.Page = PAGE_GRAPH;
	DisplayLCD.isON = true;
	DisplayLCD.Selected_Page = PAGE_GRAPH;
	DisplayLCD.Page_transition = true;
	DisplayLCD.TurnOFF_event = false;
	DisplayLCD.TurnON_event = false;
}

void Draw_Overlay(){

    init_display_struct();

    for (size_t i = 0; i < GRAPH_PIXELS_XRANGE; i++)
    {
        TGraph.Table[i]=0;
        TGraph.Values[i]=300;
    }

    TGraph.index = 0;
    

    uint32_t image_count=0;
	asm("NOP");
	start_us_count(&image_count);
	asm("NOP");
	//LCD_DrawRGB16Image8(0,0,ILI9341_LCD_PIXEL_WIDTH, ILI9341_LCD_PIXEL_HEIGHT, image);
	//LCD_DrawRGB16Image8(4,1,231, 308, image_pinker);
	//LCD_DrawRGB16Image8(0,0,ILI9341_LCD_PIXEL_WIDTH, ILI9341_LCD_PIXEL_HEIGHT, image_leo);
	LCD_Clear(DisplayLCD.Background_color);
	asm("NOP");
	stop_us_count(&image_count);
	asm("NOP");
	//LCD_DrawBitmap(0,0, image);
	uart_printf("Time taken to send pic : %d cy - %d us\n", image_count, image_count/108);

	delay_ms(1000);
	LCD_SetFont(&Font12_7);
	LCD_SetTextColor(DisplayLCD.Text_color);
	// Overlay at the top consisting of 2 stripes
	LCD_DrawRect(0,0,240,25);
	LCD_DrawRect(0,1,240,25);
	LCD_DrawRect(0,25,240,25);
	LCD_DrawRect(0,26,240,25);
	// partition 1st stripe into 3 parts
	LCD_DrawVLine(65, 0, 25);
	LCD_DrawVLine(66, 0, 25);

	LCD_DrawVLine(155, 0, 25);
	LCD_DrawVLine(156, 0, 25);

	// Draw outline of indicators
	LCD_DrawRect(6,6,16,16);
	LCD_DrawRect(72,6,16,16);
	LCD_DrawRect(162,6,16,16);
	LCD_DrawRect(178,6,16,16);


	LCD_DisplayStringAt(25, 10, "CARD", LEFT_MODE);
	LCD_DisplayStringAt(100, 10, "SPECTRO", LEFT_MODE);
	LCD_DisplayStringAt(210, 10, "DAC", LEFT_MODE);

	// 2nd stripe

	LCD_DrawHLine(87, 65,150);
	LCD_DrawVLine(87+150, 65, 40);
	LCD_DrawHLine(3, 65+40, 87+150-3);
	LCD_DrawVLine(3, 65, 40);

	LCD_SetFont(&Font24_17);
	LCD_DisplayStringAt(0, 80, "0000.00 C", CENTER_MODE);

	LCD_SetFont(&Font12_7);
	// Bottom Tabs : Graph | Param | Lang
	LCD_DrawVLine(65, 300, 20);
	LCD_DrawVLine(66, 300, 20);

	LCD_DrawVLine(155, 300, 20);
	LCD_DrawVLine(156, 300, 20);

	LCD_DrawRect(0,300,240,20);
	LCD_DrawRect(0,301,240,20);

	Select_Graph_tab();

	DisplayLCD.Page_transition=false;

}

void Clear_Page(){
	if(!DisplayLCD.isON){
		return;
	}

	if(DisplayLCD.isON){
		LCD_SetTextColor(DisplayLCD.Background_color);
    	LCD_FillRect(0, OVERLAY_MIDDLE_Y1, ILI9341_LCD_PIXEL_WIDTH, OVERLAY_MIDDLE_Y2-OVERLAY_MIDDLE_Y1);
    	LCD_SetTextColor(DisplayLCD.Text_color);
	}
}

void Select_Graph_tab(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_FillRect(1, 301, 64,19);
	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(67, 301, 89,19);
	LCD_FillRect(157, 301, 82,19);
	LCD_SetTextColor(DisplayLCD.Text_color);

}

void Select_Param_tab(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_FillRect(67, 301, 89,19);
	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(1, 301, 64,19);
	LCD_FillRect(157, 301, 82,19);
	LCD_SetTextColor(DisplayLCD.Text_color);

}

void Select_Lang_tab(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_FillRect(157, 301, 82,19);
	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(67, 301, 89,19);
	LCD_FillRect(1, 301, 64,19);
	LCD_SetTextColor(DisplayLCD.Text_color);

}

void Draw_Select_Param(){
	if(!DisplayLCD.isON){
		return;
	}
	
	DisplayLCD.Page_transition=false;

	Clear_Page();

	LCD_SetTextColor(LCD_COLOR_ORANGE);
	LCD_FillRect(67, 301, 89,19);
	LCD_SetTextColor(DisplayLCD.Text_color);

	LCD_DrawHLine(87, 115,150);
	LCD_DrawVLine(87+150, 115, 35);
	LCD_DrawHLine(3, 115+35, 87+150-3);
	LCD_DrawVLine(3, 115, 35);
	
	

	LCD_DrawHLine(87, 160,150);
	LCD_DrawVLine(87+150, 160, 290-160);
	LCD_DrawHLine(3, 290, 87+150-3);
	LCD_DrawVLine(3, 160, 290-160);

}


void Select_Param(int16_t index){
	if(!DisplayLCD.isON){
		return;
	}

	if(DisplayLCD.Page_transition){
		Draw_Select_Param();
	}
	else{
		//clear name & desc fields
		LCD_SetTextColor(DisplayLCD.Background_color);
    	LCD_FillRect(5, 125, ILI9341_LCD_PIXEL_WIDTH-10, 20);
		LCD_FillRect(5, 165, ILI9341_LCD_PIXEL_WIDTH-8, 285-165);
    	LCD_SetTextColor(DisplayLCD.Text_color);
	}
	

	
	LCD_SetFont(DisplayLCD.Font_Default);


}

void Draw_Config_Param(){
	if(!DisplayLCD.isON){
		return;
	}

	DisplayLCD.Page_transition=false;

	//clear name & desc fields
	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(5, 125, ILI9341_LCD_PIXEL_WIDTH-10, 20);
	LCD_FillRect(5, 165, ILI9341_LCD_PIXEL_WIDTH-10, 285-165);
	LCD_FillRect(5, 151, 86-5, 155+12-151);
	LCD_SetTextColor(DisplayLCD.Text_color);


	LCD_SetFont(&Font16_11);
	LCD_SetFont(DisplayLCD.Font_Default);
}


void Config_Param(){
	if(!DisplayLCD.isON){
		return;
	}

	if(DisplayLCD.Page_transition){
		Draw_Config_Param();
	}

	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(28, 200, ILI9341_LCD_PIXEL_WIDTH-56, 20+16+20);
	LCD_SetTextColor(DisplayLCD.Text_color);

	LCD_SetFont(&Font16_11);
	LCD_SetFont(DisplayLCD.Font_Default);

	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_SetTextColor(DisplayLCD.Text_color);
	
}

void Save_Param(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetTextColor(DisplayLCD.Text_color);
}

void Cant_Save_Param(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(5, 250, ILI9341_LCD_PIXEL_WIDTH-10, 20);
	LCD_SetTextColor(DisplayLCD.Text_color);


}

void Save_Param_Succ(){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(5, 250, ILI9341_LCD_PIXEL_WIDTH-10, 20);
	LCD_SetTextColor(DisplayLCD.Text_color);

	LCD_SetTextColor(DisplayLCD.Text_color);
}

void Draw_Select_Lang(){
	if(!DisplayLCD.isON){
		return;
	}

	DisplayLCD.Page_transition=false;

	Clear_Page();

	LCD_SetTextColor(LCD_COLOR_ORANGE);
	LCD_FillRect(157, 301, 82,19);
	LCD_SetTextColor(DisplayLCD.Text_color);

	LCD_DrawHLine(87, 115,150);
	LCD_DrawVLine(87+150, 115, 35);
	LCD_DrawHLine(3, 115+35, 87+150-3);
	LCD_DrawVLine(3, 115, 35);

}

void Select_Lang(int16_t index){
	if(!DisplayLCD.isON){
		return;
	}

	if(DisplayLCD.Page_transition){
		Draw_Select_Lang();
	}
	else{
		//clear name & desc fields
		LCD_SetTextColor(DisplayLCD.Background_color);
    	LCD_FillRect(5, 125, ILI9341_LCD_PIXEL_WIDTH-10, 20);
    	LCD_SetTextColor(DisplayLCD.Text_color);
	}

	LCD_SetFont(&Font16_11);
	char pstr[23];
	LCD_DisplayStringAt(0, 125, pstr, CENTER_MODE);
	LCD_SetFont(DisplayLCD.Font_Default);

}

void Cant_Load_Lang(){
	if(!DisplayLCD.isON){
		return;
	}
	//Display_Status("CANNOT LOAD LANG : NO CARD", LCD_COLOR_RED);
}

void Select_Page(){
	if(!DisplayLCD.isON){
		return;
	}

	switch (DisplayLCD.Selected_Page)
	{
	case PAGE_GRAPH:
		Select_Graph_tab();
		break;

	case PAGE_PARAM:
		Select_Param_tab();
		break;

	case PAGE_LANG:
		Select_Lang_tab();
		break;
	
	default:
		break;
	}
}

void Draw_Graph_Axis(){
	if(!DisplayLCD.isON){
		return;
	}

	DisplayLCD.Page_transition=false;

    
	Clear_Page();

    // Page 1 : Graph
	LCD_DrawHLine(20, 290, 210);
	LCD_DrawLine(230, 290, 225, 287);
	LCD_DrawLine(230, 290, 225, 293);
	LCD_DisplayChar(232, 285, 't');

	LCD_DrawVLine(20, 110, 180);
	LCD_DrawLine(20, 110, 17, 115);
	LCD_DrawLine(20, 110, 23, 115);

/*
	LCD_SetFont(&Font8_5);
	LCD_DisplayStringAt(0, 115, "1100", LEFT_MODE);
	LCD_DisplayStringAt(0, 285, "300", LEFT_MODE);
    
*/
    LCD_SetFont(&Font8_5);
    char MV[5], mv[5];
    snprintf(MV, 5, "%d", TGraph.max_value);
    snprintf(mv, 5, "%d", TGraph.min_value);
	LCD_DisplayStringAt(0, 115, MV, LEFT_MODE);
	LCD_DisplayStringAt(0, 285, mv, LEFT_MODE);
    LCD_SetFont(DisplayLCD.Font_Default);
	
	
}

void Update_Temperature(FLOAT value){
	if(!DisplayLCD.isON){
		return;
	}

	LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(10, 80, 220, 24);
	LCD_SetTextColor(DisplayLCD.Text_color);
	LCD_SetFont(DisplayLCD.Font_Temperature);
	char str[10];
	snprintf(str, 10,"%.2f C", value);
	LCD_DisplayStringAt(0, 80, str, CENTER_MODE);
	LCD_SetFont(DisplayLCD.Font_Default);
}

/**
 * @brief Update graph with a new value.
 * Takes about 7.5 ms
 * @param value Temperature
 */
void Update_Graph(FLOAT value){
	if(!DisplayLCD.isON){
		return;
	}

	if(DisplayLCD.Page_transition){
		Draw_Graph_Axis();
	}

	LCD_SetTextColor(DisplayLCD.Background_color);
	for (size_t i = 0; i < GRAPH_PIXELS_XRANGE-1; i++)
		{
			LCD_DrawLine(GRAPH_PIXELS_XSTART-i, GRAPH_PIXELS_YSTART-TGraph.Table[i],
						GRAPH_PIXELS_XSTART-i-1, GRAPH_PIXELS_YSTART-TGraph.Table[i+1]);

		}
	

	for (size_t i = GRAPH_PIXELS_XRANGE-1; i > 0; i--)
	{
		TGraph.Table[i]=TGraph.Table[i-1];
		TGraph.Values[i]=TGraph.Values[i-1];
	}
	FLOAT x= ((value-TGraph.min_value)*GRAPH_PIXELS_YRANGE)/TGraph.range;

	TGraph.Table[0]= (uint32_t) x;
	TGraph.Values[0]= (uint32_t) roundf(value);

	MaxMin1D_32(TGraph.Values, GRAPH_PIXELS_XRANGE, &(TGraph.max_value_inSET), NULL, &(TGraph.min_value_inSET), NULL);
	uint32_t UpperBound = TGraph.max_value_inSET/100;
	uint32_t LowerBound = TGraph.min_value_inSET/100;
	UpperBound *= 100;
	LowerBound *= 100;

	UpperBound += 100;
	LowerBound -= 100;
	
	//boundary check

	if(UpperBound != TGraph.max_value || LowerBound!=TGraph.min_value){
		Scale_Graph(UpperBound, LowerBound);
	}

	LCD_SetTextColor(DisplayLCD.Graph_color);
	for (size_t i = 0; i < GRAPH_PIXELS_XRANGE-1; i++)
	{
		LCD_DrawLine(GRAPH_PIXELS_XSTART-i, GRAPH_PIXELS_YSTART-TGraph.Table[i],
						GRAPH_PIXELS_XSTART-i-1, GRAPH_PIXELS_YSTART-TGraph.Table[i+1]);
	}

	LCD_SetTextColor(DisplayLCD.Text_color);
	
}

void Scale_Graph(uint32_t max_v, uint32_t min_v){
    if(!DisplayLCD.isON){
		return;
	}

    TGraph.max_value=max_v;
    TGraph.min_value=min_v;
    TGraph.range=max_v-min_v;

    for (size_t i = 0; i < GRAPH_PIXELS_XRANGE; i++)
    {
        FLOAT x = (((FLOAT) TGraph.Values[i]-TGraph.min_value)*GRAPH_PIXELS_YRANGE)/TGraph.range;
        TGraph.Table[i] = (uint32_t) x;
    }

    //redraw Y axis
    LCD_SetTextColor(DisplayLCD.Background_color);
	LCD_FillRect(0, 115, 19, 8);
	LCD_FillRect(0, 285, 19, 8);
	LCD_SetTextColor(DisplayLCD.Text_color);

    LCD_SetFont(&Font8_5);
    char MV[5], mv[5];
    snprintf(MV, 5, "%d", max_v);
    snprintf(mv, 5, "%d", min_v);
	LCD_DisplayStringAt(0, 115, MV, LEFT_MODE);
	LCD_DisplayStringAt(0, 285, mv, LEFT_MODE);
    LCD_SetFont(DisplayLCD.Font_Default);
}


void Test_Colors(){
    LCD_Clear(LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font16_11);
	LCD_DisplayStringAt(100,100, "BLUE",LEFT_MODE);	
	delay_ms(2000);

	
	LCD_Clear(LCD_COLOR_CYAN);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DisplayStringAt(100,100, "CYAN",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_GRAY);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayStringAt(100,100, "GRAY",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_GREEN);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DisplayStringAt(100,100, "GREEN",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_MAGENTA);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DisplayStringAt(100,100, "MAGENTA",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_RED);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DisplayStringAt(100,100, "RED",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayStringAt(100,100, "WHITE",LEFT_MODE);
	delay_ms(2000);

	LCD_Clear(LCD_COLOR_YELLOW);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayStringAt(100,100, "YELLOW",LEFT_MODE);
	delay_ms(2000);

}