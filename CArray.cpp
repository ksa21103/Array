// CArray.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string_view>
#include <random>
#include <algorithm>

#include "CArray.h"

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
      }
      printArray(intVector20);
    }

    // 1.2. Упорядочивание получившегося набора по возрастанию
    {
      printTask("1.2. Упорядочивание получившегося набора по возрастанию : ");

      std::sort(intVector20.begin(), intVector20.end());

      printArray(intVector20);
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

      printArray(intVector20);
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
      }

      printArray(intVector20);
    }

    // 1.5. Очистка контейнера
    {
      printTask("1.5. Очистка контейнера : ");

      intVector20.clear();

      printArray(intVector20);
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

        strVector.emplace_back(std::move(val));
      }

      printArray(strVector);
    }

    // 2.2. Упорядочивание получившегося набора по возрастанию
    {
      printTask("2.2. Упорядочивание получившегося набора по возрастанию : ");
      
      std::sort(strVector.begin(), strVector.end());

      printArray(strVector);
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

      printArray(strVector);
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
      }

      printArray(strVector);
    }
  }

  return 0;
}

