#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Utils.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::recognize()
{
#ifdef SIMULATOR
	char str[] = "ABCDE";
	touchgfx_printf("recognize %s\n", str);
	Unicode::strncpy(txtResultBuffer, str, TXTRESULT_SIZE);
	txtResult.invalidate();
#else
	char str[] = "ABCDE";
	Unicode::strncpy(txtResultBuffer, str, TXTRESULT_SIZE);
	txtResult.invalidate();
#endif
}