/**
 * Get openmp version by C++ code.
 * Reference: https://stackoverflow.com/questions/1304363/how-to-check-the-version-of-openmp-on-linux
 * 
 * Note: you may also set the environment:
 *    export OMP_DISPLAY_ENV=TRUE
 * 
 * Then, in any of your openmp program, when it runs, you will see contents like the following:
OPENMP DISPLAY ENVIRONMENT BEGIN
   _OPENMP='201611'
  [host] OMP_AFFINITY_FORMAT='OMP: pid %P tid %i thread %n bound to OS proc set {%A}'
  [host] OMP_ALLOCATOR='omp_default_mem_alloc'
  [host] OMP_CANCELLATION='FALSE'
  [host] OMP_DEFAULT_DEVICE='0'
  [host] OMP_DISPLAY_AFFINITY='FALSE'
  [host] OMP_DISPLAY_ENV='TRUE'
  [host] OMP_DYNAMIC='FALSE'
  [host] OMP_MAX_ACTIVE_LEVELS='1'
  [host] OMP_MAX_TASK_PRIORITY='0'
  [host] OMP_NESTED: deprecated; max-active-levels-var=1
  [host] OMP_NUM_THREADS: value is not defined
  [host] OMP_PLACES: value is not defined
  [host] OMP_PROC_BIND='false'
  [host] OMP_SCHEDULE='static'
  [host] OMP_STACKSIZE='8M'
  [host] OMP_TARGET_OFFLOAD=DEFAULT
  [host] OMP_THREAD_LIMIT='2147483647'
  [host] OMP_TOOL='enabled'
  [host] OMP_TOOL_LIBRARIES: value is not defined
  [host] OMP_WAIT_POLICY='PASSIVE'
OPENMP DISPLAY ENVIRONMENT END
 */

#include <iostream>
#include <omp.h>
#include <unordered_map>

using namespace std;

int main()
{

    printf("------------------------\n");
    std::unordered_map<unsigned,std::string> map{
        {200505,"2.5"},{200805,"3.0"},{201107,"3.1"},{201307,"4.0"},{201511,"4.5"},{201811,"5.0"},{202011,"5.1"}};
    std::cout << "We have OpenMP " << map.at(_OPENMP) << ".\n";
    std::cout << "_OPENMP is " << _OPENMP << std::endl;
    printf("------------------------\n");

    return 0;
}