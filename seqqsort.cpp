#include <list>
#include <algorithm>
#include <iostream>
#include <future>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
  if (input.empty())
  {
    return input;
  }
  std::list<T> result;
  result.splice(result.begin(), input, input.begin());
  T const& pivot = *result.begin();
  
  auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t){return t<pivot;});
  
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
  
  auto new_lower(sequential_quick_sort(std::move(lower_part)));
  auto new_higher(sequential_quick_sort(std::move(input)));
  
  result.splice(result.end(), new_higher);
  result.splice(result.begin(), new_lower);
  return result;
}

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
  if (input.empty())
  {
    return input;
  }
  std::list<T> result;
  result.splice(result.begin(), input, input.begin());
  T const& pivot = *result.begin();
  
  auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t){return t<pivot;});
  
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
  
  std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));
  auto new_higher(parallel_quick_sort(std::move(input)));
  
  result.splice(result.end(), new_higher);
  result.splice(result.begin(), new_lower.get());
  return result;
}


int main()
{
  std::list<int> list1 = {4, 5, 2, 3, 9, 8, -1, 0};
  
  auto list2 = sequential_quick_sort<int>(list1);
  
  for (auto it = list2.begin(), end = list2.end(); it != end; it++)
  {
    std::cout << *it << " " <<  std::endl;
  }
  
  auto list3 = sequential_quick_sort<int>(list1);
  
  for (auto it = list3.begin(), end = list3.end(); it != end; it++)
  {
    std::cout << *it << " " <<  std::endl;
  }
}