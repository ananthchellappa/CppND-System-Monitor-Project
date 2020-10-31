#include "ncurses_display.h"
#include "system.h"
#include <thread>
#include <iostream>

auto f_dummy  = [](){ std::cout << "Test";};

int main() {
  System system;
  NCursesDisplay::Display(system);

  std::thread t_pinfo( &NCursesDisplay::Display, system );
  t_pinfo.join();

  // std::thread t1( [](){ std::cout << "Test";} ); // works fine
  // t1.join();

}

