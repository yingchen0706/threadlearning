#include <iostream>
#include <thread>
#include <numeric>
#include <iterator>
#include <vector>
#include <algorithm>

template<typename Iterator, typename T>
struct accumulate_block
{
  void operator()(Iterator first, Iterator last, T& result)
  {
    result = std::accumulate(first, last, result);
  }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
  unsigned long const length = std::distance(first, last);
  
  if (!length)
    return init;
  
  unsigned long const min_per_thread = 25;
  unsigned long const max_thread = (length + min_per_thread - 1) / min_per_thread;
  
  unsigned long const hardware_threads = std::thread::hardware_concurrency();
  std::cout << hardware_threads << std::endl;
  
  unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_thread);
  
  unsigned long const block_size = length / num_threads;
  
  std::vector<T> results(num_threads);
  std::vector<std::thread> threads(num_threads - 1);
  
  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_threads - 1); i++)
  {
    Iterator block_end = block_start;
    std::advance(block_end, block_size); // can we use +?
    threads[i] = std::thread(accumulate_block<Iterator, T>(),
    block_start,
    block_end,
    std::ref(results[i]));
    block_start = block_end;
  }
  
  accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
  
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
  
  return std::accumulate(results.begin(), results.end(), init);
}

int main()
{
  std::vector<int> elements(100);
  
  for (int i = 0; i < 100; i++)
  {
    elements[i] = i;
  }
  
  int res = 0;
  res = parallel_accumulate<std::vector<int>::iterator, int>(elements.begin(), elements.end(), res);
  std::cout << res << std::endl;
}