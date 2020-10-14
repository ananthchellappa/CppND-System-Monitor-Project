#include "processor.h"
#include "linux_parser.h" // to use the file name vars

// debug
#include <iostream>

using std::string;
using LinuxParser::kProcDirectory;
using LinuxParser::kStatFilename;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string line;
  string key;
  long user;
  long total;  // use implicit conversion..
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirq;
  long steal;
  long guest;
  long guest_nice;
  long nonIdle;
  float total_load;
  float CPU_usage;

  std::regex ptrn ("\\s+");  // one or more whitespace
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      line = std::regex_replace ( line, ptrn, " ");
      std::istringstream linestream(line);
      linestream >> key; // first line is all we need for this one /proc/stat
        if ("cpu" == key) {
            linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
            idle += iowait;
            nonIdle = user + nice + system + irq + softirq + steal + guest + guest_nice;
            total = idle + nonIdle;
            total_load = total - prevTotal_;
            // std::cout << idle - prevIdle_ << std::endl;
            CPU_usage = ( total_load -  idle + prevIdle_ ) / total_load;
            prevIdle_ = idle;
            prevTotal_ = total;
            return CPU_usage;
        }
    }
  }
  return 0.33;  // debug
}
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
