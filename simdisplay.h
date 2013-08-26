#ifndef GTKMM_EXAMPLE_MYAREA_H
#define GTKMM_EXAMPLE_MYAREA_H

#include "world.h"
#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#define R1  255./255., 153./255., 153./255.
#define R2  255./255., 102./255., 102./255.
#define R3  255./255., 051./255., 051./255.
#define R4  255./255., 0./255., 0./255.
#define R5  191./255., 0./255., 0./255.
#define R6  128./255., 0./255., 0./255.
#define R7  64./255., 0./255., 0./255.


class SimDisplay : public Gtk::DrawingArea
{
public:
  SimDisplay();
  virtual ~SimDisplay();

protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool perform_step();
  virtual bool refresh();

private:
  World *world;
  int step_count;
  Cairo::RefPtr<Cairo::PsSurface> surface;

};

#endif // GTKMM_EXAMPLE_MYAREA_H
