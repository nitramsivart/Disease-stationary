#include "simdisplay.h"
#include <cmath>

// Constructor creates the world which contains all simulation information.
SimDisplay::SimDisplay()
{
  step_count = 100;
  world = new World();
  surface = Cairo::PsSurface::create("image.ps", 1000, 1000);

    
  #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Connect the signal handler if it isn't already a virtual method override:
  //signal_draw().connect(sigc::mem_fun(*this, &SimDisplay::on_draw), false);
  #endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::refresh), 200 );
}

SimDisplay::~SimDisplay()
{
  //std::string filename = "image.pdf";
  //surface->write_to_pdf(filename);
  surface->finish();
}

// When the mouse is clicked the simulation is progressed.
bool SimDisplay::on_button_press_event(GdkEventButton* event) {
  step_count = 0;
  printf("on_button_press_event\n");
  //buttonpress = event;
  //Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::perform_step), 500 );
  refresh();
  perform_step();
}

bool SimDisplay::perform_step() {
    world->step();
    refresh();
}

bool SimDisplay::refresh() {
  printf("refresh\n");
  gdk_window_invalidate_rect(get_window()->gobj(), NULL, true);
  get_window()->invalidate(true);
}

bool SimDisplay::on_expose_event(GdkEventExpose* event) {
  printf("expose\n");

  Cairo::RefPtr<Cairo::Context> c = Cairo::Context::create(surface);
  on_draw(c);
  //get_window()->create_cairo_context());
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
    const int width = 1000;//allocation.get_width();
    const int height = 1000;//allocation.get_height();
    int lesser = MIN(width, height);

    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    cr->set_line_width(lesser * 0.00001 * 1);  // outline thickness changes
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
        cr->set_source_rgba(0.8,0.8,0.8, 1);    // partially translucent
      else if(people[i].status < 0) 
        cr->set_source_rgba(R7, 1.0);
      else 
        switch(people[i].status) {
          case 6:
            cr->set_source_rgba(R5, 1); // dark green
            break;
          case 5:
            cr->set_source_rgba(R4, 1); // green
            break;
          case 4:
            cr->set_source_rgba(R3, 1); // yellow
            break;
          case 3:
            cr->set_source_rgba(R2, 1); // orange
            break;
          case 2:
            cr->set_source_rgba(R1, 1); // red
            break;
          default:
            cr->set_source_rgba(R6, 1); // red
        }
        //cr->set_source_rgba(((float)people[i].status)/DAYS_INFECTED, 0.0, 0.0, .7);
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

