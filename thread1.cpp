#include <iostream>
#include <thread>

void hello()
{
  std::cout << "Hello Concurrent world\n";
}

int main()
{
  std::thread t(hello);
  t.join();
}