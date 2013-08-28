#include "simdisplay.h"
#include <cmath>

// Constructor creates the world which contains all simulation information.
SimDisplay::SimDisplay()
{
  step_count = 45;
  world = new World();
  int grid_wid = sqrt(world->get_count());
  surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, grid_wid, grid_wid);
  //surface = Cairo::PsSurface::create("image-3.ps", grid_wid, grid_wid);
  for(int i = 0; i < step_count; i++)
    world->step();

    
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
  std::string filename = "image.png";
  surface->write_to_png(filename);
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
    printf("performing step\n");
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
    int grid_wid = sqrt(world->get_count());
    const int width = grid_wid;//allocation.get_width();
    const int height = grid_wid;//allocation.get_height();
    int lesser = MIN(width, height);

    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    cr->set_line_width(0);
    //cr->set_line_width(0*lesser * 0.00001 * 1);  // outline thickness changes
                                        // with window size

    
    /* Draw all of our people */
    World::person *people = world->get_people();
    int i;
    for(i = 0; i < world->get_count(); i++) {
      cr->save();
      xc = people[i].x * grid_wid;
      yc = people[i].y * grid_wid;
      double sq = 1.03 * (width / (sqrt(world->get_count())));
      cr->rectangle(xc, yc, 1, 1); // rectangle
      //cr->rectangle(xc - sq/2, yc - sq/2, sq, sq); // rectangle
      //cr->arc(xc, yc, lesser / 800.0, 0.0, 2.0 * M_PI); // full circle
      if(people[i].status < 0) {
        printf("neg status\n");
        cr->set_source_rgba(R7, 1.0);
      }
      else if(people[i].status == 0)
        cr->set_source_rgba(0.8,0.8,0.8, 1); // Susceptible
      else if(people[i].status <= 5)
        cr->set_source_rgba(R1, 1); // red
      else if(people[i].status <= 10)
        cr->set_source_rgba(R2, 1); // red
      else if(people[i].status <= 15)
        cr->set_source_rgba(R3, 1); // red
      else if(people[i].status <= 20)
        cr->set_source_rgba(R4, 1); // red
      else if(people[i].status <= 25)
        cr->set_source_rgba(R5, 1); // red
      else if(people[i].status <= 30)
        cr->set_source_rgba(R6, 1); // red
      else
        cr->set_source_rgba(R7, 1); // red
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

