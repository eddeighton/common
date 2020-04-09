
#include "common/processID.hpp"

#include <boost/process.hpp>

namespace Common
{

size_t getProcessID()
{
    return boost::this_process::get_id();
}

}
