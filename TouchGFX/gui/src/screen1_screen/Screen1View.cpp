#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/Color.hpp>

#ifdef SIMULATOR
#include <touchgfx/Utils.hpp>
#else
#include "dbger.h"
#endif

static uint32_t randomNum(uint32_t min, uint32_t max) {
  return rand() % (max - min + 1) + min;
}

Screen1View::Screen1View()
{
#ifdef SIMULATOR	
  touchgfx_printf("sizeof(touchgfx::Line): %d bytes\n", sizeof(touchgfx::Line));
#else
	LOG_DBG((char*)"sizeof(touchgfx::Line): %d bytes\n", sizeof(touchgfx::Line));
#endif
	touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);
  linePainter.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
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

void Screen1View::addLine()
{
  touchgfx::Line* line = new touchgfx::Line();
  lines.push_back(line);

  line->setPosition(100, 100, 300, 300);
  line->setPainter(linePainter);
  line->setLineWidth(5);
  line->setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
  add(*line);

  line->setStart(randomNum(0, 300), randomNum(0, 300));
  line->setEnd(randomNum(0, 300), randomNum(0, 300));
  line->invalidate();
	
#ifdef SIMULATOR	
  touchgfx_printf("add line %d\n", lines.size());
#else
	LOG_DBG((char*)"add line %d\n", lines.size());
#endif
}

void Screen1View::deleteLine()
{
#ifdef SIMULATOR	
  touchgfx_printf("delete %d lines\n", lines.size());
#else
	LOG_DBG((char*)"delete %d lines\n", lines.size());
#endif
  for(std::list<touchgfx::Line*>::iterator it = lines.begin(); it != lines.end(); it++) {
    (*it)->setVisible(false);
    (*it)->invalidate();
    remove(**it);
    delete *it;
  }
  lines.clear();
}

void Screen1View::draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	touchgfx::Line* line = new touchgfx::Line();
  lines.push_back(line);

  line->setPosition(100, 100, 300, 300);
  line->setPainter(linePainter);
  line->setLineWidth(5);
  line->setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
  add(*line);

  line->setStart(x1, y1);
  line->setEnd(x2, y2);
  line->invalidate();
}