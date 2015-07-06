#include <iostream>
#include <mutex>
#include <climits>
#include <thread>

class hierarchical_mutex
{
  std::mutex internal_mutex;
  unsigned long const hierarchy_value;
  unsigned long previous_hierarchy_value;
  
  static thread_local unsigned long this_thread_hierarchy_value;
  
  void check_for_hierarchy_violation()
  {
    if (this_thread_hierarchy_value <= hierarchy_value)
    {
      std::cout << "mutex hierarchy violated" << std::endl;
      throw std::logic_error("mutex hierarchy violated");
    }
  }
  
  void update_hierarchy_value()
  {
    previous_hierarchy_value = this_thread_hierarchy_value;
    this_thread_hierarchy_value = hierarchy_value;
  }
  
public:
  explicit hierarchical_mutex(unsigned long value):
    hierarchy_value(value),
    previous_hierarchy_value(0)
    {
      
    }
  
  void lock()
  {
    check_for_hierarchy_violation();
    internal_mutex.lock();
    update_hierarchy_value();
    std::cout << " lock for " << this_thread_hierarchy_value << std::endl;
  }
  
  void unlock()
  {
    std::cout << " unlock for " << this_thread_hierarchy_value << std::endl;
    this_thread_hierarchy_value = previous_hierarchy_value;
    internal_mutex.unlock();
  }
  
  bool try_lock()
  {
    check_for_hierarchy_violation();
    if (!internal_mutex.try_lock())
      return false;
    update_hierarchy_value();
    return true;
  }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);

hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);

int do_low_level_stuff()
{
  return 0;
}

int low_level_func()
{
  std::lock_guard<hierarchical_mutex> lk(low_level_mutex);
  return do_low_level_stuff();
}

void high_level_stuff(int some_param)
{
  
}

void high_level_func()
{
  std::lock_guard<hierarchical_mutex> lk(high_level_mutex);
  high_level_stuff(low_level_func());
}

void thread_a()
{
  high_level_func();
}

hierarchical_mutex other_mutex(100);

void do_other_stuff()
{
  
}

void other_stuff()
{
  high_level_func();
  do_other_stuff();
}

void thread_b()
{
  std::lock_guard<hierarchical_mutex> lk(other_mutex);
  other_stuff();
}

int main()
{

  std::thread th1(thread_a);
  th1.join();
  std::thread th2(thread_b);
  th2.join();
  
}