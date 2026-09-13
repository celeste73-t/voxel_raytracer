#include "app.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << "Starting\n";
  App app;

  std::cout << "App launched\n";
  
  while (app.running)
  {
    app.update();
  }
  app.write_session_report();

  return 0;
}