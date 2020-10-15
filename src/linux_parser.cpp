#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <regex>  // for the RAM from /proc/meminfo
#include <filesystem>

#include "linux_parser.h"

// debug
#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace fs = std::filesystem;

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
  std::regex rgx ("^\\d+$"); // only digits
  std::smatch match;
  for( auto& p: fs::directory_iterator( kProcDirectory.c_str() ) ) {
    string file( p.path().filename() );
    if ( std::regex_search( file, match, rgx ) ) {
      int pid = stoi( file );
      pids.push_back(pid);
    }
  }
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
long LinuxParser::UpTime() { 
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
vector<long> LinuxParser::CpuUtilization( int pid ) {  // this is not the one for the aggregate but for each process..
  // why is the return type a vector and string??? 
  // why is there no parameter - should take pid! added that
  // int --> long, long // total_time and start_time
  string line;
  string key;
  long starttime;
  long utime;
  long stime;
  long cutime;
  long cstime;
  std::ifstream filestream( kProcDirectory + to_string(pid) + kStatFilename );
  if ( filestream.is_open() ) {
    while ( std::getline(filestream, line) ) {
      std::istringstream linestream(line);
      int i = 0;
      linestream >> key >> key >> key;
      while ( linestream >> starttime && ( i++ < 9 ) ){};   // here starttime is a dummy buffer
      linestream >> utime >> stime >> cutime >> cstime;
      i = 0;
      while ( linestream >> starttime && ( i++ < 4 ) ){};
      return { utime + stime + cutime + cstime , starttime };
    }
  }
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
string LinuxParser::Command(int pid ) {
  string line;
  std::ifstream filestream( kProcDirectory + std::to_string(pid) + kCmdlineFilename );
  if ( filestream.is_open() ) {
    std::getline( filestream , line );
    return line;
  }
  return "Error opening cmdline";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid ) {
  // int --> string looking in /proc/<pid>/status
  string line;
  string ram;
  std::regex rgx ("^VmSize:\\s+(\\d+)"); // kB
  std::smatch match;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  // std::cout << kProcDirectory + std::to_string(pid) + "/" + kStatusFilename;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
        if ( std::regex_search( line, match, rgx ) ) {
          ram =  to_string(stof( match.str(1) ) / 1000.0 );
          rgx = "^(\\d+(\\.\\d\\d)?)";
          std::regex_search( ram, match, rgx );
          return match.str( 1 );  // how else to truncate to 2 decimal places?
        }
    }
  }
  return "VmSize error";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid ) { 
  // int --> string looking in /proc/<pid>/status
  string line;
  std::regex rgx ("^Uid:\\s+(\\d+)"); 
  std::smatch match;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  // std::cout << kProcDirectory + std::to_string(pid) + "/" + kStatusFilename;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
        if ( std::regex_search( line, match, rgx ) ) {
          return match.str(1);
        }
    }
  }
  return "Uid error";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User( int pid ) { 
  // string --> string  looking in /etc/passwd
  string uid = Uid( pid );
  string line;
  std::regex rgx ("^(\\w+):\\w+:(\\d+)"); 
  std::smatch match;
  std::ifstream filestream(kPasswordPath);
  // std::cout << kProcDirectory + std::to_string(pid) + "/" + kStatusFilename;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
        if ( std::regex_search( line, match, rgx ) && uid == match.str(2) ) {
          return match.str(1);
        }
    }
  }
  return "Uid error";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime( int pid ){
  string line;
  string key;
  long value;
  std::ifstream filestream( kProcDirectory + to_string(pid) + kStatFilename );
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      int i = 0;
      linestream >> key >> key >> key;
      while ( linestream >> value && ( i++ < 18 ) ){};
      return value / sysconf(_SC_CLK_TCK);
    }
  }
  return -1 ;
}