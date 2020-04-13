//@group Core

#ifndef EN_ENTRYPOINT_H
#define EN_ENTRYPOINT_H

#include <fstream>
#include <exception>

extern Engine::Application * Engine::CreateApplication();

int main(int argc, char** argv)
{
  //Clear crash report file
  std::ofstream ofs;
  ofs.open(CRASH_REPORT_FILE, std::ofstream::out | std::ofstream::trunc);
  ofs.close();
  
  Engine::Application * app(nullptr);

  try
  {
    app = Engine::CreateApplication();
  }
  catch (std::exception & e)
  {
    std::ofstream ofs(CRASH_REPORT_FILE);
    ofs << "Game failed to initialise: " << e.what();
    goto epilogue;
  }
  catch (...)
  {
    std::ofstream ofs(CRASH_REPORT_FILE);
    ofs << "Game failed to intialise, no error reported.";
    goto epilogue;
  }
  
  try
  {
    app->Run();
  }
  catch (std::exception & e)
  {
    std::ofstream ofs(CRASH_REPORT_FILE);
    ofs << "Game has thrown exception: " << e.what();
  }
  catch (...)
  {
    std::ofstream ofs(CRASH_REPORT_FILE);
    ofs << "Game has thrown an unknown exception";
  }
  
  epilogue:
  delete app;
  return 0;
}

#endif