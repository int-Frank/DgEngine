#include "TestHarness.h"

#include <iostream>
#include <string>
#include <fstream>

#define RESULTS_FILE "unit-test-results.txt"

int main(int argc, char* argv[])
{
  std::string resultsFile(RESULTS_FILE);
  for (int i = 0; i < argc - 1; ++i)
  {
    if (strcmp(argv[i], "-out") == 0)
    {
      resultsFile = std::string(argv[i + 1]);
    }
  }

  //Redirect std::cerr to file
  std::filebuf CLOG_NEW_BUF;
  CLOG_NEW_BUF.open(resultsFile, std::ios::out);
  std::streambuf* CLOG_OLD_BUF = std::clog.rdbuf(&CLOG_NEW_BUF);

  TestResult tr;
  TestRegistry::runAllTests(tr);

  std::clog.rdbuf(CLOG_OLD_BUF);
  int failCount = tr.FailureCount();

  return failCount;
}