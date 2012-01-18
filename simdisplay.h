#ifndef GTKMM_EXAMPLE_MYAREA_H
#define GTKMM_EXAMPLE_MYAREA_H

#include "world.h"
#include <gtkmm/drawingarea.h>

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
};

#endif // GTKMM_EXAMPLE_MYAREA_H
