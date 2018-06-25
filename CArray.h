#pragma once
#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <cassert>
#include <iterator>

///////////////////////////////////////////////////////////////////////////////
template <typename TData, typename TAllocator = std::allocator<TData>>
class CArray
{
public: // Interface

  // Конструктор по умолчанию
  CArray();

  // Копирующий конструктор
  CArray(
      const CArray & _array
    );

  // Перемещающий конструктор
  CArray(
      CArray && _array
    );

  // Деструктор
  ~CArray();

  // Добавить элемент в конец массива
  void push_back(
      const TData & _value
    );

  // Сконструировать элемент в конец массива
  template <typename T>
  void emplace_back(
      T&& _value
    );

  // Сконструировать элемент в конец массива
  template <class... Args>
  void emplace_back(
      Args&&... args
    );

  // Добавить элемент в массив по заданному индексу
  void insert(
      unsigned int  _index,
      const TData & _value
    );

  // Удалить элемент массива по заданному индексу
  void erase(
      unsigned int _index
    );

  // Очистить массив
  void clear();

  // Получить размер массива
  unsigned int size() const;

  // Определить что массив пустой
  bool empty() const;

  // Получить элемент массива по заданному индексу
  TData & operator[](
      unsigned int _index
    );

  // Получить элемент массива по заданному индексу
  const TData & operator[](
      unsigned int _index
    ) const;

  // Итераторы
  template <typename ContainerType, typename DataType>
  class iterator_base
  {
    friend CArray;

  public:

    // iterator traits
    using difference_type   = int;
    using value_type        = DataType;
    using pointer           = DataType *;
    using reference         = DataType &;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit iterator_base(
        ContainerType* _arrayContainer,
        unsigned int   _index
      );

    iterator_base(
        const iterator_base & _it
      );

    iterator_base& operator++();
    iterator_base  operator++(int) const;
    iterator_base& operator+(
        int _offset
      );

    iterator_base& operator--();
    iterator_base  operator--(int) const;
    iterator_base& operator-(
        int _offset
      );

    int operator-(
        const iterator_base & _it
      ) const;

    bool operator==(
        const iterator_base& _it
      ) const;

    bool operator!=(
        const iterator_base& _it
      ) const;

    bool operator<(
        const iterator_base& _it
      ) const;

    DataType& operator*() const;

    DataType& operator->() const;

    DataType* getPtr() const;

  protected:

    unsigned int GetIndex() const;

  protected:

    ContainerType * m_arrayContainer = nullptr;
    unsigned int    m_index = 0;
  };

  using iterator       = typename iterator_base<CArray, TData>;
  using const_iterator = typename iterator_base<const CArray, const TData>;

  iterator        begin();
  const_iterator  begin()   const;
  const_iterator  cbegin()  const;

  iterator        end();
  const_iterator  end()     const;
  const_iterator  cend()    const;

  // Добавить элемент в массив по заданному индексу
  void insert(
      iterator _pos,
      const TData & _value
    );

  // Удалить диапазон элементов
  void erase(
      const iterator & _itFrom,
      const iterator & _itTo
    );

protected:  // Методы

  template <typename TItemType, typename TAllocatorType>
  class MemoryBuf
  {
    TAllocator    m_allocator;
    unsigned int  m_allocatedObjectsCount = 0;

    TItemType *   m_buf    = nullptr;    //< Область памяти для хранения данных
    unsigned int  m_size   = 0;          //< Количество созданных элементов

  public:

    MemoryBuf(
        unsigned int _destCapacity
      );

    ~MemoryBuf();

    void swap(
        MemoryBuf& _other
      );

    // Получить размер
    unsigned int size() const;

     // Определить наличие свободного места
    bool hasFreeSpace() const;

    // Подготовить контейнер для создания нового элемента
    void prepareToAddNewItem();

#ifdef _DEBUG
    // Проверить валидность адреса памяти
    bool isValidAddr(TData * _addr, size_t _bufSize);
#endif

    // Получить указатель на элемент по инлексу
    TItemType * getPData(
        unsigned int _index
      );

    // Получить указатель на элемент по инлексу
    const TItemType * getPData(
        unsigned int _index
      ) const;

    // Получить правую ссылку на элемент по инлексу
    TItemType&& getPDataRValue(
        unsigned int _index
      );

    // Деструкция объектов
    void destroyObjects();

    // Деструкция объектов
    void destroyObjects(
        unsigned int _indexFrom,
        unsigned int _indexTo
      );

    // Перемещение объектов
    void moveObjectsFrom(
        MemoryBuf<TItemType, TAllocator> & _dataSrc
      );

    // Копирование в новый объект
    void constructFrom(
        unsigned int _indexTo,
        MemoryBuf<TItemType, TAllocator>& _other,
        unsigned int _indexFrom
      );

    template <typename T>
    void constructFromObj(
        TData * _destRawBuf,
        T&& _srcObj
      );
  };

  // Передвинуть перемещаемые данные
  void insertCopyData(
      unsigned int  _index,
      const TData & _value
    );

  // Удалить элемент массива по заданному индексу
  void eraseImpl(
      unsigned int _indexFrom,
      unsigned int _indexTo
    );

protected: // Attributes

  MemoryBuf<TData, TAllocator> m_data;
};

namespace std
{
  template <typename TData, typename TAllocator>
  auto begin(CArray<TData, TAllocator>& _array)
  {
    return _array.begin();
  }

  template <typename TData, typename TAllocator>
  auto end(CArray<TData, TAllocator>& _array)
  {
    return _array.end();
  }
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
CArray<TData, TAllocator>::CArray()
  : m_data(0)
{
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
CArray<TData, TAllocator>::CArray(
    const CArray & _array
  )
  : m_data(_array.m_data.capacity)
{
  copyObjects(_array.m_data, m_data);

  m_data.size = _array.size;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
CArray<TData, TAllocator>::CArray(CArray && _array)
{
  m_data.swap(_array.m_data);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
CArray<TData, TAllocator>::~CArray()
{
  m_data.destroyObjects();
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::push_back(
    const TData & _value
  )
{
  m_data.prepareToAddNewItem();

  m_data.constructFromObj(m_data.getPData(m_data.size()), _value);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename T>
void
CArray<TData, TAllocator>::emplace_back(
    T&& _value
  )
{
  m_data.prepareToAddNewItem();

  m_data.constructFromObj(m_data.getPData(m_data.size()), std::forward<T>(_value));
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <class ...Args>
void
CArray<TData, TAllocator>::emplace_back(
    Args && ...args
  )
{
  m_data.prepareToAddNewItem();

  m_data.constructFromObj(m_data.getPData(m_data.size), std::forward<Args>(args)...);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::insert(
    unsigned int _index, 
    const TData & _value
  )
{
  if (!m_data.hasFreeSpace())
  {
    MemoryBuf<TData, TAllocator> newData(std::max<unsigned long>(m_data.size() * 2, 1));

    unsigned int fromIdx = 0;
    unsigned int toIdx = 0;

    for ( ; fromIdx < _index; ++fromIdx, ++toIdx)
    {
      newData.constructFrom(toIdx, m_data, fromIdx);
    }

    newData.constructFromObj(newData.getPData(toIdx++), _value);

    for (auto dataSize = m_data.size(); fromIdx < dataSize; ++fromIdx, ++toIdx)
    {
      newData.constructFrom(toIdx, m_data, fromIdx);
    }

    m_data.swap(newData);
    newData.destroyObjects();
  }
  else
  {
    insertCopyData(_index, _value);
  }
}

//----------------------------------------------------------------------------//
// Добавить элемент в массив по заданному индексу
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::insertCopyData(
    unsigned int  _index,
    const TData & _value
  )
{
  unsigned int toIdx = m_data.size();
  unsigned int fromIdx = toIdx - 1;

  for ( ; _index <= fromIdx; --fromIdx, --toIdx)
  {
    *(m_data.getPData(toIdx)) = *(m_data.getPData(fromIdx));

    if (fromIdx == 0)
    {
      break;
    }
  }

  m_data.constructFromObj(m_data.getPData(_index), _value);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::insert(
    iterator _pos,
    const TData & _value
  )
{
  insert(_pos.getPtr() - m_data.m_buf, _value);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::erase(
    unsigned int _index
  )
{
  eraseImpl(_index, _index + 1);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::erase(
    const iterator & _itFrom,
    const iterator & _itTo
  )
{
  eraseImpl(_itFrom.GetIndex(), _itTo.GetIndex());
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::eraseImpl(
    unsigned int _indexFrom,
    unsigned int _indexTo
  )
{
  assert(_indexFrom <= _indexTo);
  if (_indexFrom == _indexTo)
  {
    // Начальная и конечная границы удаляемого диапазона совпадают - просто выходим
    return;
  }

  const auto dataSize = m_data.size();

  // Копирование/перемещение
  if ((_indexTo + 1) < dataSize)
  {
    unsigned int fromIdx   = _indexTo;
    unsigned int fromIdxTo = std::min<unsigned int>(dataSize, fromIdx + _indexTo - _indexFrom);
    unsigned int toIdx     = fromIdx + _indexFrom - _indexTo;

    for ( ; fromIdx < fromIdxTo; ++fromIdx, ++toIdx)
    {
      if constexpr (!std::is_trivially_copy_assignable<TData>::value)
      {
        if constexpr (std::is_move_assignable<TData>::value)
        {
          *m_data.getPData(toIdx) = std::move(*m_data.getPData(fromIdx));
        }
        else
        {
          *m_data.getPData(toIdx) = *m_data.getPData(fromIdx);
        }
      }
      else
      {
        memcpy(m_data.getPData(toIdx), m_data.getPData(fromIdx), sizeof(TData));
      }

      ++_indexFrom;
    }

    _indexTo   = fromIdxTo;
  }

  m_data.destroyObjects(_indexFrom, _indexTo);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
void
CArray<TData, TAllocator>::clear()
{
  m_data.destroyObjects();
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
unsigned int
CArray<TData, TAllocator>::size() const
{
  return m_data.size();
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
bool
CArray<TData, TAllocator>::empty() const
{
  return m_data.size == 0;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
TData&
CArray<TData, TAllocator>::operator[](
    unsigned int _index
  )
{
  return *m_data.getPData(_index);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
const TData&
CArray<TData, TAllocator>::operator[](
    unsigned int _index
  ) const
{
  return *m_data.getPData(_index);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::iterator
CArray<TData, TAllocator>::begin()
{
  return iterator(this, 0);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::const_iterator
CArray<TData, TAllocator>::begin() const
{
  return cbegin();
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::iterator
CArray<TData, TAllocator>::end()
{
  return iterator(this, m_data.size());
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::const_iterator
CArray<TData, TAllocator>::end() const
{
  return cend();
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::const_iterator
CArray<TData, TAllocator>::cbegin() const
{
  return const_iterator(this, 0);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
typename CArray<TData, TAllocator>::const_iterator
CArray<TData, TAllocator>::cend() const
{
  return const_iterator(this, m_data.size());
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::iterator_base(
    ContainerType* _arrayContainer,
    unsigned int   _index
  )
  : m_arrayContainer(_arrayContainer),
    m_index         (_index)
{
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::iterator_base(
  const iterator_base & _it
  )
  : m_arrayContainer(_it.m_arrayContainer),
    m_index         (_it.m_index)
{
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator++()
{
  operator+(1);

  return *this;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator++(
    int
  ) const
{
  iterator_base<ContainerType, DataType> tmp(it);
  operator++();
  return tmp;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator+(
    int _offset
  )
{
  auto newIndex = m_index + _offset;
  if (0 <= newIndex && newIndex <= m_arrayContainer->size())
  {
    m_index = newIndex;
  }

  return *this;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator--()
{
  operator-(1);

  return *this;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator--(
    int
  ) const
{
  iterator_base<ContainerType, DataType> tmp(it);
  operator--();
  return tmp;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
typename CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator-(
    int _offset
  )
{
  auto newIndex = m_index - _offset;
  if (0 <= newIndex && newIndex < m_arrayContainer->size())
  {
    m_index = newIndex;
  }
  else
  {
    m_arrayContainer = nullptr;
    m_index = 0;
  }

  return *this;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
int
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator-(
    const iterator_base & _it
  ) const
{
  return m_index - _it.m_index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
bool
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator==(
    const iterator_base<ContainerType, DataType>& _it
  ) const
{
  return m_arrayContainer == _it.m_arrayContainer && m_index == _it.m_index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
bool
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator!=(
  const iterator_base& _it
  ) const
{
  return !operator==(_it);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
bool
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator<(
    const iterator_base& _it
  ) const
{
  return m_index < _it.m_index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
DataType&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator*() const
{
  return (*m_arrayContainer)[m_index];
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
DataType&
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::operator->() const
{
  return (*m_arrayContainer)[m_index];
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
DataType *
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::getPtr() const
{
  return &(*m_arrayContainer)[m_index];
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename ContainerType, typename DataType>
unsigned int
CArray<TData, TAllocator>::iterator_base<ContainerType, DataType>::GetIndex() const
{
  return m_index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::MemoryBuf(
    unsigned int _destCapacity
  )
{
  m_allocatedObjectsCount = _destCapacity;
  if (m_allocatedObjectsCount)
  {
    m_buf = m_allocator.allocate(m_allocatedObjectsCount);
  }
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::~MemoryBuf()
{
  assert(size() == 0);

  if (m_allocatedObjectsCount)
  {
    m_allocator.deallocate(m_buf, m_allocatedObjectsCount);
    m_buf = nullptr;
    m_allocatedObjectsCount = 0;
  }
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::swap(
    MemoryBuf & _other
  )
{
  std::swap(m_allocator,             _other.m_allocator);
  std::swap(m_allocatedObjectsCount, _other.m_allocatedObjectsCount);
  std::swap(m_buf,                   _other.m_buf);
  std::swap(m_size,                  _other.m_size);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
unsigned int
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::size() const
{
  return m_size;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
bool
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::hasFreeSpace() const
{
  return m_size < m_allocatedObjectsCount;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::prepareToAddNewItem()
{
  if (!hasFreeSpace())
  {
    MemoryBuf<TData, TAllocatorType> newData(std::max<unsigned long>(size() * 2, 1));

    newData.moveObjectsFrom(*this);

    newData.swap(*this);

    newData.destroyObjects();
  }
}

#ifdef _DEBUG
//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
bool
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::isValidAddr(
    TData * _addr,
    size_t _bufSize
  )
{
  bool bResult = m_buf <= _addr && ((char*)_addr + _bufSize) <= (char*)(m_buf + m_allocatedObjectsCount);

  return bResult;
}
#endif


//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
typename TItemType*
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::getPData(
    unsigned int _index
  )
{
  return m_buf + _index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
const typename TItemType*
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::getPData(
    unsigned int _index
  ) const
{
  return m_buf + _index;
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
TItemType&&
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::getPDataRValue(
    unsigned int _index
  )
{
  return std::move(*(m_buf + _index));
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::destroyObjects()
{
  destroyObjects(0, m_size);
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::destroyObjects(
    unsigned int _indexFrom,
    unsigned int _indexTo
  )
{
  assert(_indexFrom <= _indexTo);
  if (_indexFrom == _indexTo)
  {
    // Начальная и конечная границы удаляемого диапазона совпадают - просто выходим
    return;
  }

  if (_indexFrom < _indexTo)
  {
    if constexpr (!std::is_trivially_destructible<TItemType>::value)
    {
      for (auto index = _indexFrom; index < _indexTo; ++index)
      {
        TItemType* pitem = m_buf + index;
        pitem->~TData();
      }
    }

    m_size -= _indexTo - _indexFrom;
  }
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::moveObjectsFrom(
    MemoryBuf<TItemType, TAllocator> & _dataSrc
  )
{
  if (_dataSrc.size() > 0)
  {
    unsigned int index = 0;
    TData* pItemTo = m_buf;
    while (index < _dataSrc.size())
    {
      constructFrom(index, _dataSrc, index);
      ++index;
    }

    m_size = _dataSrc.size();
  }
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::constructFrom(
    unsigned int _indexTo,
    MemoryBuf<TItemType, TAllocator> & _other,
    unsigned int _indexFrom
  )
{
  constructFromObj(getPData(_indexTo), _other.getPDataRValue(_indexFrom));
}

//----------------------------------------------------------------------------//
template <typename TData, typename TAllocator>
template <typename TItemType, typename TAllocatorType>
template <typename T>
void
CArray<TData, TAllocator>::MemoryBuf<TItemType, TAllocatorType>::constructFromObj(
    TData * _destRawBuf,
    T&& _srcObj
  )
{
  assert(isValidAddr(_destRawBuf, sizeof(TData)));

  if constexpr (!std::is_trivially_constructible<TItemType>::value)
  {
    if constexpr (std::is_copy_constructible<TItemType>::value
                  || std::is_move_constructible<TItemType>::value)
    {
      new (_destRawBuf) TData(std::forward<T>(_srcObj));
    }
    else
    {
      *(new (_destRawBuf) TData()) = std::forward<T>(_srcObj);
    }
  }
  else
  {
    memcpy(_destRawBuf, &_srcObj, sizeof(TItemType));
  }

  ++m_size;
}

//----------------------------------------------------------------------------//
