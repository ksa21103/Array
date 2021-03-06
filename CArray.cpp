// CArray.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string_view>
#include <random>
#include <algorithm>
#include <memory>

#include "CArray.h"

////////////////////////////////////////////////////////////////////////////////
// Раскомментировать дефайн для сравнения работы с вектором из STL
////////////////////////////////////////////////////////////////////////////////
#define COMPARE_WORK

#ifdef COMPARE_WORK
#   include <vector>
#endif

////////////////////////////////////////////////////////////////////////////////
// Раскомментировать дефайн для проверки срабатывания конструкторов/деструкторов в контейнере
////////////////////////////////////////////////////////////////////////////////
//#define TEST_CONSTRUCTOR_DESTRUCTOR_WORK
#ifdef TEST_CONSTRUCTOR_DESTRUCTOR_WORK
  class MyStringType : public std::string
  {
    static size_t m_counstructors;
    static size_t m_destruectors;
  public:

    MyStringType()
    {
      ++m_counstructors;
    }

    MyStringType(
        const MyStringType& _other
      )
      : std::string(_other)
    {
      ++m_counstructors;
    }

    MyStringType(
        MyStringType&& _other
      )
      : std::string(std::move(_other))
    {
      ++m_counstructors;
    }

    ~MyStringType()
    {
      ++m_destruectors;
    }

    template <typename T>
    MyStringType& operator=(
        T&& _other
      )
    {
      std::string::operator=(std::forward<T>(_other));
      return *this;
    }

    static bool testState()
    {
      return m_counstructors == m_destruectors;
    }
  };
  size_t MyStringType::m_counstructors = 0;
  size_t MyStringType::m_destruectors  = 0;

  typedef MyStringType  TStringType;

#else

  typedef std::string   TStringType;

#endif

////////////////////////////////////////////////////////////////////////////////
// Раскомментировать дефайн для проверки выделения/освобождения памяти
////////////////////////////////////////////////////////////////////////////////
//#define TEST_MEMORY
#ifdef TEST_MEMORY

  #include <set>

  using AllocationsSet = std::set<std::pair<void*, size_t>>;

  template <typename T>
  class MyTestAllocator : public std::allocator<T>
  {
    static std::set<std::pair<void*, size_t>> m_allocations;

  public:

    T * allocate(
        std::size_t _n,
        const void * _hint)
    {
      auto result = std::allocator<T>::allocate(_n, _hint);

      m_allocations.insert({ result, _n });

      return result;
    }

    T* allocate(
        std::size_t _n
      )
    {
      auto result = std::allocator<T>::allocate(_n);

      m_allocations.insert({ result, _n });

      return result;
    }

    void deallocate(
        T * _p,
        std::size_t _n
      )
    {
      std::allocator<T>::deallocate(_p, _n);

      m_allocations.erase({ _p, _n });
    }

    static bool testState()
    {
      return m_allocations.empty();
    }
  };

  template <typename T>
  std::set<std::pair<void*, size_t>> MyTestAllocator<T>::m_allocations;

  template <typename T>
  using MY_ALLOCATOR = MyTestAllocator<T>;

#else

  template <typename T>
  using MY_ALLOCATOR = std::allocator<T>;

#endif

//----------------------------------------------------------------------------//
// Вывод строки
template <typename SomeStream>
SomeStream& operator << (
    SomeStream & _stream,
    std::string_view str
  )
{
  _stream << "'" << std::data(str) << "'";

  return _stream;
}

//----------------------------------------------------------------------------//
// Вывод контейнера
template <typename TArray>
void printArray(
    const TArray& _array
  )
{
  std::cout << "size = " << _array.size() << "\t";
  for (auto item : _array)
  {
    std::cout << item << "; ";
  }
  std::cout << std::endl;
}

//----------------------------------------------------------------------------//
void printTask(
    std::string_view _taskName
  )
{
  std::cout << std::endl;
  std::cout << std::data(_taskName);
  std::cout << std::endl;
}

//----------------------------------------------------------------------------//
#ifdef COMPARE_WORK
// Сравнение содержимого контейнеров
template <typename TArray1, typename TArray2>
bool compareArrays(
    const TArray1& _array1,
    const TArray2& _array2
  )
{
  if (_array1.size() != _array2.size())
    return false;

  auto it1    = _array1.begin();
  auto it1End = _array1.end();
  auto it2    = _array2.begin();
  auto it2End = _array2.end();

  for ( ; it1 < it1End; ++it1, ++it2)
  {
    if (*it1 != *it2)
    {
      return false;
    }
  }

  return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// class RamdomWorldParams - формирование случайного слова указазанной длины
class RamdomWorldParams
{
public:

  RamdomWorldParams(
      size_t _worldMaxLen
    )
    : m_randomEngine     (m_random()),
      m_word_len         (1, _worldMaxLen),
      m_uniform_dist_char(0, 25)
  {
  }

  std::string makeWord()
  {
    const auto len = m_word_len(m_randomEngine);

    
    std::string str;
    str.reserve(len);

    for (auto chIndex = 0; chIndex < len; ++chIndex)
    {
      auto charIndex = m_uniform_dist_char(m_randomEngine);
      char ch = 'a' + charIndex;
      assert('a' <= ch && ch <= 'z');
      str.append(1, ch);
    }

    return str;
  }

private:
  std::random_device                 m_random;
  std::default_random_engine         m_randomEngine;
  std::uniform_int_distribution<int> m_word_len;
  std::uniform_int_distribution<int> m_uniform_dist_char;
};

//----------------------------------------------------------------------------//
int main()
{
  setlocale(LC_ALL, "Russian");

  // 1. Работа с числами (int)
  {
    CArray<int>       intVector20;
#ifdef COMPARE_WORK
    std::vector<int>  intVector20Check;
#endif

    // 1.1. Добавление в цикле 20 случайных чисел в диапазоне от 0 до 100
    {
      std::random_device r;
      std::default_random_engine e1(r());
      std::uniform_int_distribution<int> uniform_dist(0, 100);

      printTask("1.1. Добавление в цикле 20 случайных чисел в диапазоне от 0 до 100 : ");
      for (size_t index = 0; index < 20; ++index)
      {
        auto val = uniform_dist(e1);
        intVector20.push_back(val);
#ifdef COMPARE_WORK
        intVector20Check.push_back(val);
#endif
      }
      printArray(intVector20);
#ifdef COMPARE_WORK
      printArray(intVector20Check);
      std::cout << (compareArrays(intVector20, intVector20Check) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 1.2. Упорядочивание получившегося набора по возрастанию
    {
      printTask("1.2. Упорядочивание получившегося набора по возрастанию : ");
      std::sort(intVector20.begin(), intVector20.end());
#ifdef COMPARE_WORK
      std::sort(intVector20Check.begin(), intVector20Check.end());
#endif
      printArray(intVector20);
#ifdef COMPARE_WORK
      printArray(intVector20Check);
      std::cout << (compareArrays(intVector20, intVector20Check) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 1.3. Удаление каждого второго элемента
    {
      size_t counter = 0;
      printTask("1.3. Удаление каждого второго элемента : ");
      intVector20.erase(
        std::remove_if(intVector20.begin(), intVector20.end(),
                       [&counter](const auto&)
                       {
                         return (counter++ % 2) ? true : false;
                       } ),
        intVector20.end());
#ifdef COMPARE_WORK
      counter = 0;
      intVector20Check.erase(
        std::remove_if(intVector20Check.begin(), intVector20Check.end(),
                       [&counter](const auto&)
                       {
                         return (counter++ % 2) ? true : false;
                       } ),
        intVector20Check.end());
#endif
      printArray(intVector20);
#ifdef COMPARE_WORK
      printArray(intVector20Check);
      std::cout << (compareArrays(intVector20, intVector20Check) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 1.4. Вставка 10 случайных чисел в диапазоне от 0 до 100 на случайные позиции
    {
      printTask("1.4. Вставка 10 случайных чисел в диапазоне от 0 до 100 на случайные позиции : ");

      std::random_device r;
      std::default_random_engine e1(r());
      std::uniform_int_distribution<int> uniform_dist(0, 100);
      
      for (size_t index = 0; index < 20; ++index)
      {
        std::uniform_int_distribution<int> uniform_dist_pos(0, intVector20.size());
        unsigned long pos = uniform_dist_pos(e1);

        auto val = uniform_dist(e1);
        intVector20.insert(pos, val);
#ifdef COMPARE_WORK
        intVector20Check.insert(intVector20Check.begin() + pos, val);
#endif
      }

      printArray(intVector20);
#ifdef COMPARE_WORK
      printArray(intVector20Check);
      std::cout << (compareArrays(intVector20, intVector20Check) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 1.5. Очистка контейнера
    {
      printTask("1.5. Очистка контейнера : ");

      intVector20.clear();
#ifdef COMPARE_WORK
      intVector20Check.clear();
#endif

      printArray(intVector20);
#ifdef COMPARE_WORK
      printArray(intVector20Check);
      std::cout << (compareArrays(intVector20, intVector20Check) ? "OK" : "FAIL") << std::endl;
#endif
    }
  }
  
  // 2. Работа с объектами (std::string)
  {
    CArray<std::string> strVector;
    std::vector<std::string> strVectorCheck;
    RamdomWorldParams   randomWordFactory(10);

    // 2.1. Добавление в цикле 15 случайно выбранных слов, состоящих из латинских букв в нижнем регистре
    {
      printTask("2.1. Добавление в цикле 15 случайно выбранных слов, состоящих из латинских букв в нижнем регистре : ");

      for (size_t index = 0; index < 15; ++index)
      {
        std::string val = randomWordFactory.makeWord();
#ifdef COMPARE_WORK
        std::string val1 = val;
#endif

        strVector.emplace_back(std::move(val));
#ifdef COMPARE_WORK
        strVectorCheck.emplace_back(std::move(val1));
#endif
      }

      printArray(strVector);
#ifdef COMPARE_WORK
      printArray(strVectorCheck);
      std::cout << (compareArrays(strVector, strVectorCheck) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 2.2. Упорядочивание получившегося набора по возрастанию
    {
      printTask("2.2. Упорядочивание получившегося набора по возрастанию : ");
      
      std::sort(strVector.begin(), strVector.end());
#ifdef COMPARE_WORK
      std::sort(strVectorCheck.begin(), strVectorCheck.end());
#endif

      printArray(strVector);
#ifdef COMPARE_WORK
      printArray(strVectorCheck);
      std::cout << (compareArrays(strVector, strVectorCheck) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 2.3. Удаление каждого слова, включающего в себя любую из букв a, b, c, d, e
    {
      const char chars[] = { 'a', 'b', 'c', 'd', 'e' };

      printTask("2.3. Удаление каждого слова, включающего в себя любую из букв a, b, c, d, e : ");

      strVector.erase(
        std::remove_if(strVector.begin(), strVector.end(),
                       [&](const auto& _str)
                       {
                         for (auto ch : chars)
                         {
                           if (std::string::npos != _str.find(ch))
                           {
                             return true;
                           }
                         }
                         return false;
                       } ),
        strVector.end());

#ifdef COMPARE_WORK
      strVectorCheck.erase(
        std::remove_if(strVectorCheck.begin(), strVectorCheck.end(),
                       [&](const auto& _str)
                       {
                         for (auto ch : chars)
                         {
                           if (std::string::npos != _str.find(ch))
                           {
                             return true;
                           }
                         }
                         return false;
                       } ),
        strVectorCheck.end());
#endif

      printArray(strVector);
#ifdef COMPARE_WORK
      printArray(strVectorCheck);
      std::cout << (compareArrays(strVector, strVectorCheck) ? "OK" : "FAIL") << std::endl;
#endif
    }

    // 2.4. Вставка 3 новых случайно выбранных слов на случайные позиции
    {
      printTask("2.4. Вставка 3 новых случайно выбранных слов на случайные позиции : ");

      std::random_device r;
      std::default_random_engine e1(r());

      for (size_t index = 0; index < 3; ++index)
      {
        std::uniform_int_distribution<int> uniform_dist_pos(0, strVector.size());
        unsigned long pos = uniform_dist_pos(e1);

        auto val = randomWordFactory.makeWord();
        
        strVector.insert(pos, val);
#ifdef COMPARE_WORK
        strVectorCheck.insert(strVectorCheck.begin() + pos, val);
#endif
      }

      printArray(strVector);
#ifdef COMPARE_WORK
      printArray(strVectorCheck);
      std::cout << (compareArrays(strVector, strVectorCheck) ? "OK" : "FAIL") << std::endl;
#endif
    }
  }

#ifdef TEST_CONSTRUCTOR_DESTRUCTOR_WORK
  // Проверим количество срабатываний конструкторов/деструкторов
  assert(MyStringType::testState());
#endif

#ifdef TEST_MEMORY
  // Проверим, что вся выделенная память освобождена
  assert(MY_ALLOCATOR<int>::testState());
#endif

  return 0;
}

