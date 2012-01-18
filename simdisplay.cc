#include "simdisplay.h"
#include <cairomm/context.h>
#include <cmath>
#include <math.h>
#include <time.h>

// Constructor creates the world which contains all simulation information.
SimDisplay::SimDisplay()
{
  for(int i = 0; i < 100; i++) {
    x[i] = 0;
    y[i] = .5;
  }
  replace = 0;
  
    
  #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Connect the signal handler if it isn't already a virtual method override:
  signal_draw().connect(sigc::mem_fun(*this, &SimDisplay::on_draw), false);
  #endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED

  srand(time(NULL));
}

SimDisplay::~SimDisplay()
{
}

// When the mouse is clicked the simulation is progressed.
bool SimDisplay::on_button_press_event(GdkEventButton* event) {
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::perform_step), 25 );
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::refresh), 10 );
}

bool SimDisplay::perform_step() {
  for(int i = 0; i < 100; i++) {
    x[i] = x[i] - 1;
  }
  x[replace] = 100;
  y[replace] = rand()/((float)RAND_MAX + 1);
  replace = (replace + 1) % 100;
}

bool SimDisplay::refresh() {
  //gdk_window_invalidate_rect(get_window()->gobj(), NULL, true);
  get_window()->invalidate(true);
}

bool SimDisplay::on_expose_event(GdkEventExpose* event) {
  on_draw(get_window()->create_cairo_context());
  return true;
}

// When the window is first displayed or resized this event is called.
// We redraw the window
bool SimDisplay::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  // This is where we draw on the window
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  int lesser = MIN(width, height);

  // coordinates for the center of the window
  int xc, yc;
  xc = width / 2;
  yc = height / 2;

  cr->set_line_width(lesser * 0.00001);  // outline thickness changes
  // with window size

  /* Draw all of our people */
  int i;
  for(i = 0; i < 100; i++) {
    cr->save();
    xc = (x[i]/100.0) * width;
    yc = y[i] * height;
    cr->arc(xc, yc, lesser / 200.0, 0.0, 2.0 * M_PI); // full circle
    cr->set_source_rgba(0.0, 0.0, 1.0, 0.3);    // partially translucent
    cr->fill_preserve();
    cr->restore();  // back to opaque black
    cr->stroke();
  }
  return true;
}

