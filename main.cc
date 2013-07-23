#include "simdisplay.h"
#include <gtkmm/main.h>
#include <gtkmm/window.h>

// This starts the window, which starts the simulation
int main(int argc, char** argv)
{
   Gtk::Main kit(argc, argv);

   Gtk::Window win;
   win.set_title("Plague Simulator");
   win.set_default_size(500, 500);

   SimDisplay area;
   win.add(area);
   area.show();
  
   // Make it so that the DrawingArea accepts click events
   area.add_events((Gdk::EventMask)(1 << 8));

   Gtk::Main::run(win);

   return 0;
}

