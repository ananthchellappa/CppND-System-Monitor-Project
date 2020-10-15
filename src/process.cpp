#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    long uptime = LinuxParser::UpTime();    // going by https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    vector<long> tot_start = LinuxParser::CpuUtilization( pid_ );
    float seconds = uptime - tot_start[1]/sysconf( _SC_CLK_TCK );
    return tot_start[0] / sysconf( _SC_CLK_TCK ) / seconds;
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command( pid_ );
}

// TODO: Return this process's memory utilization
string Process::Ram() const { 
    return LinuxParser::Ram( pid_ );
}

// TODO: Return the user (name) that generated this process
string Process::User() { //return string(); }
    // return LinuxParser::User( LinuxParser::Uid(pid_) );
    return LinuxParser::User( pid_ );
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime( pid_ );
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a ) const { //return true; }
    if( sort_crit_ ) {
        return this->CpuUtilization() < a.CpuUtilization() ? true : false;
    } else {
        if ( std::stof( this->Ram() ) < std::stof( a.Ram() ) )
            return true;
        else
            return false;
    }
}

// bool Process::sort_crit_ = true;
