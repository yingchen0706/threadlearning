#include <iostream>
#include <thread>

class background_task
{
public:
  void operator() () const
  {
    std::cout << "test" << std::endl;  
  }
};

int main()
{
  background_task f;
  std::thread my_thread(f);
  my_thread.join();
  std::thread my_thread2((background_task()));
  my_thread2.join();
  std::thread my_thread3{background_task()};
  my_thread3.join();
}