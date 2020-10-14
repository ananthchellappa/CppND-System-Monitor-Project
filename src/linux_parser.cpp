#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <regex>  // for the RAM from /proc/meminfo

#include "linux_parser.h"

// debug
#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
// string LinuxParser::OperatingSystem_old() { // inelegant solution
//   string line;
//   string key;
//   string value;
//   std::ifstream filestream(kOSPath);
//   if (filestream.is_open()) {
//     while (std::getline(filestream, line)) {
//       std::replace(line.begin(), line.end(), ' ', '_');
//       std::replace(line.begin(), line.end(), '=', ' ');
//       std::replace(line.begin(), line.end(), '"', ' ');
//       std::istringstream linestream(line);
//       while (linestream >> key >> value) {
//         if (key == "PRETTY_NAME") {
//           std::replace(value.begin(), value.end(), '_', ' ');
//           return value;
//         }
//       }
//     }
//   }
//   return value;
// }

// RE-DONE - a better way
string LinuxParser::OperatingSystem() {
  string line;
  std::regex rgx ("PRETTY_NAME=\"([^\"]+)"); 
  std::smatch match;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
        if ( std::regex_search( line, match, rgx ) ) {
          return match.str(1);
        }
    }
  }
  return "Error : check search string!";
}

// DONE: An example of how to read data from the filesystem /proc/version
string LinuxParser::Kernel() {
  string os, kernel;  // the line is linux version name and we want 'name'
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> os >> kernel;   // tweaked
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());   // how to know where DIR is defined?
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { //return 0.0; }
  string line;
  string key;
  long value;
  float total;  // use implicit conversion..
  float free;
  std::regex ptrn ("\\s+");  // one or more whitespace
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      line = std::regex_replace ( line, ptrn, " ");
      std::istringstream linestream(line);
      // std::cout << line ; // debug
      while (linestream >> key >> value) {
        if ("MemTotal:" == key) {
          total = value;
        }
        if ("MemFree:" == key) {
          free = value;

          return 1 - free/total;
        }
      }
    }
  }
  return 0.33;  // debug
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { //return 0; }
  long int uptime;  // first number from /proc/uptime
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename );
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;   // tweaked
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {  // this is not the one for the aggregate
   return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { //return 0; } // kStatFilename
  string line;
  string key;
  int value;
  std::ifstream filestream( kProcDirectory + kStatFilename );
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { //return 0; }
  string line;
  string key;
  int value;
  std::ifstream filestream( kProcDirectory + kStatFilename );
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }