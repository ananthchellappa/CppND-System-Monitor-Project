#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    long prevIdle_;
    long prevTotal_;    // going by https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
};

#endif