#include "simdisplay.h"
#include <cairomm/context.h>
#include <cmath>

// Constructor creates the world which contains all simulation information.
SimDisplay::SimDisplay()
{
  step_count = 100;
  world = new World();
    
  #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Connect the signal handler if it isn't already a virtual method override:
  signal_draw().connect(sigc::mem_fun(*this, &SimDisplay::on_draw), false);
  #endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::refresh), 200 );
}

SimDisplay::~SimDisplay()
{
}

// When the mouse is clicked the simulation is progressed.
bool SimDisplay::on_button_press_event(GdkEventButton* event) {
  step_count = 0;
  printf("on_button_press_event\n");
  //buttonpress = event;
  //Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::perform_step), 500 );
  //refresh();
  perform_step();
}

bool SimDisplay::perform_step() {
    world->step();
    refresh();
}

bool SimDisplay::refresh() {
  printf("refresh\n");
  //gdk_window_invalidate_rect(get_window()->gobj(), NULL, true);
  get_window()->invalidate(true);
}

bool SimDisplay::on_expose_event(GdkEventExpose* event) {
  printf("expose\n");
  on_draw(get_window()->create_cairo_context());
  //if(buttonpress != NULL)
    //perform_step();
  return true;
}

// When the window is first displayed or resized this event is called.
// We redraw the window
bool SimDisplay::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  printf("drawing\n");
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
    World::person *people = world->get_people();
    int i;
    for(i = 0; i < world->get_count(); i++) {
      cr->save();
      xc = people[i].x * width;
      yc = people[i].y * height;
      cr->arc(xc, yc, lesser / 200.0, 0.0, 2.0 * M_PI); // full circle
      if(people[i].status == 0)
        cr->set_source_rgba(0.0, 0.0, 1.0, 0.7);    // partially translucent
      else if(people[i].status < 0) 
        cr->set_source_rgba(0.0, 1.0, 0.0, 0.7);
      else 
        cr->set_source_rgba(((float)people[i].status)/DAYS_INFECTED, 0.0, 0.0, .7);
      cr->fill_preserve();
      cr->restore();  // back to opaque black
      cr->stroke();
    }

  /*if(step_count < 5) {
    perform_step();
    step_count++;
  }*/

  return true;
}

