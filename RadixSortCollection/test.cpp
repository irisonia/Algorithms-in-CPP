#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

#include "radix_sort_collection_api.h"

class SomeClass
{
   friend ostream &operator<<(ostream &o, const SomeClass &sc);
public:

   SomeClass(size_t id = 0)
   {
       // random values for checking the sort
       char name[4] = {0};
       for(size_t i = 0; i < (sizeof(name) - 1); ++i) {
           name[i] = 'a' + (rand() % (('z' - 'a') + 1));
           if (rand() % 2) {
               name[i] = toupper(name[i]);
           }
       }

       m_num = id;
       m_str = name;
   }

   SomeClass& operator=(const SomeClass &&rhs)
   {
       m_num = rhs.m_num;
       m_str = std::move(rhs.m_str);
       return *this;
   }

   SomeClass(const SomeClass &&rhs)
   {
       m_num = rhs.m_num;
       m_str = std::move(rhs.m_str);
   }

   SomeClass(const SomeClass &rhs)
   {
       // added this method for ensuring that no
       // objects are created during radix sort

      ++ctr_cctor;

       m_num = rhs.m_num;
       m_str = rhs.m_str;
   }

   SomeClass& operator=(const SomeClass &rhs)
   {
       // aded this method for ensuring that no
       // objects are assigned to during radix sort

       ++ctr_assign;

       if (this == &rhs)
           return *this;

       m_num = rhs.m_num;
       m_str = rhs.m_str;

       return (*this);
   }

   static auto getKey(const SomeClass &f)
   {
       return f.m_num;
      //return (unsigned char)f.m_str.at(0);
   }

   static constexpr auto keySize() {
        return sizeof(std::result_of<decltype(&getKey)(const SomeClass&)>::type);
   }

   bool operator!=(const SomeClass &rhs) const
   {
       return SomeClass::getKey(*this) != SomeClass::getKey(rhs);
   }

   bool operator<(const SomeClass &rhs) const
   {
      return (getKey(*this) < getKey(rhs)) ;
   }

   static void InitTestObjectCreationsAndAssignments()
   {
      ctr_assign = 0;
      ctr_cctor = 0;
   }

   static void TestObjectCreationsAndAssignments()
   {
      if (ctr_cctor != 0) {
          cout << "Error: T objects were created during sort:" << ctr_cctor << endl;
      }
      if (ctr_assign != 0) {
          cout << "Error: T objects were assigned during sort." << ctr_assign << endl;
      }
   }

private:
   uint64_t    m_num;
   string      m_str;

   // for ensuring that no objects are created nor
   // assigned to during radix sort.
   static size_t ctr_cctor;
   static size_t ctr_assign;
};
size_t SomeClass::ctr_assign(0);
size_t SomeClass::ctr_cctor(0);

ostream &operator<<(ostream &o, const SomeClass &sc)
{
   o << "[" << sc.m_num << ", " << sc.m_str << "], ";
   return o;
}

template <class IT>
void Print(IT it, size_t sz)
{
   for (size_t i = 0; i < sz; ++it, ++i) {
       cout << *it << ", ";
   }
   cout << endl;
}

template <class T>
T GetRandIntegral(size_t max_val, bool all_positive, bool all_negative = false)
{
   // aimed to get small/big/positive/negative values, but other
   // than that, there is no special reason for the numbers in the %

   T val = rand() % max_val;

   if(!(val % 2356)) {
       val = max_val - (rand() % 255);
   }
   else if (!(val % 2815)) {
       val %= 120;
   }
   else if (!(val % 739)) {
       val = std::numeric_limits<T>::max();
   }

   if (all_negative) {
       return -val;
   }

   if (!all_positive) {
      if (!(val % 5)) {
          val = -val;
      }
      else if (!(val % 132)) {
          val = std::numeric_limits<T>::min();
      }
   }

   return val;
}

static double CalcTimeDiff(const timespec *te, const timespec *tb)
{
   return ((double)te->tv_sec + 1.0e-9*te->tv_nsec) -
           ((double)tb->tv_sec + 1.0e-9*tb->tv_nsec);
}

template<class IT1, class IT2>
static void check(IT1 container_begin, IT2 container_ok_begin, size_t sz)
{
   // make sure that radix sort was successful:
   // - compare against the result of std::sort (std::list::sort for list)
   // - check that no element is bigger than the one following it
   // - ensure that the elements did not somehow become all equal

   IT1 it = container_begin;
   IT2 it_ok = container_ok_begin;

   for(size_t i = 0 ; i < sz ; ++i, ++it, ++it_ok) {
       if (*it != *it_ok) {
           cout << "Error: Radix did not work (compared to std).\n";
           return;
       }
   }

   bool all_same = 0;
   it = container_begin;
   IT1 it_next = container_begin; ++it_next;
   for(size_t i = 0 ; i < sz - 1 ; ++i, ++it, ++it_next) {
       if (all_same && (*it != *container_begin)) {
           all_same = false;
       }

       if(*it_next < *it) {
           cout << "Error: Radix did not work (bigger element before smaller element).\n";
           return;
       }
   }

   if(all_same && (sz > 1)) {
       cout << "Error: After sort, all values were identical\n";
       return;
   }

   cout << "radix ok   ";
}

// checks the scenario where only indexes are sorted into an output
// memory, while the original array remains unchanged.
template<class T>
static void check(const T *arr, const T *arr_ok, const size_t *mem, size_t sz)
{
   for(size_t i = 0 ; i < sz ; ++i) {
       if (arr[mem[i]] != arr_ok[i]) {
           cout << "Error: Radix did not work (compared to std).\n";
           return;
       }
   }

   bool all_same = 0;
   for(size_t i = 0 ; i < sz - 1 ; ++i) {
       if (all_same && (mem[i] != mem[i+1])) {
           all_same = false;
       }

       if((arr[mem[i+1]] < arr[mem[i]])) {
           cout << "Error: Radix did not work (arr[i+1]) < arr[i]).\n";
           return;
       }
   }

   if(all_same && (sz > 1)) {
       cout << "Error: After sort, all values were identical\n";
       return;
   }

   cout << "radix ok   ";
}

template<typename T>
static void check(const std::list<T> &lst, const std::list<T> &lst_ok, size_t sz)
{
   if (lst.size() != sz) {
       cout << "Error: Wrong size for list after sort.\n";
       return;
   }

   if (lst.empty()) return;

   check(lst.begin(), lst_ok.begin(), sz);
}

template <class IT, class CREATE_ENTRY, class RADIX_CALL, class STD_CALL, class CHECK>
void TestImpl(
       IT it,
       IT it_ok,
       size_t sz,
       CREATE_ENTRY &create_entry,
       RADIX_CALL &radix_call,
       STD_CALL &std_call,
       CHECK &check_call)
{
   for (size_t i = 0; i < sz; ++i, ++it, ++it_ok) {
      create_entry(it, it_ok, i);
   }

   timespec start_radix, end_radix;
   clock_gettime(CLOCK_MONOTONIC, &start_radix);
   int radix_status = radix_call();
   clock_gettime(CLOCK_MONOTONIC, &end_radix);

   if (!radix_status) {
       timespec start_std_sort, end_std_sort;
       clock_gettime(CLOCK_MONOTONIC, &start_std_sort);
       std_call();
       clock_gettime(CLOCK_MONOTONIC, &end_std_sort);

       cout << "std::sort  " << CalcTimeDiff(&end_std_sort, &start_std_sort) << endl;
       check_call();
       cout << CalcTimeDiff(&end_radix, &start_radix) << endl;
   }
   else {
       cout << "Error: Could not allocate memory\n";
   }
}

void TestArrUserDefinedType(size_t sz)
{
   cout << "\nSorting array of " << sz << " class objects\n";

   auto arr = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto arr_ok = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);

   auto create_entry = [](SomeClass *elem1, SomeClass *elem2, size_t i) {
       *elem1 = *elem2 = SomeClass(i);
   };
   auto radix_call = [arr, sz]() {return RadixSort(arr.get(), sz, SomeClass::getKey);};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, sz]() {check(arr.get(), arr_ok.get(), sz);};

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

void TestArrUserDefinedTypeAllocHere(size_t sz)
{
   cout << "\nSorting array of " << sz << " class objects, allocation of helper memory by client\n";

   // Checking with different kinds of allocations: malloc, new, shared_ptr
   auto arr = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto arr_ok = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto mem1 = new char[sz * (sizeof(size_t) * 2)];
   // the malloc() must be last of allocations:
   auto mem2 = (char*)malloc(sz * (sizeof(size_t) * 2));

   if (! mem2) {
       delete [] mem1; mem1 = nullptr;
       cout << "Error: Could not allocate memory\n";
       return;
   }

   auto create_entry = [](SomeClass *elem1, SomeClass *elem2, size_t i) {
       *elem1 = *elem2 = SomeClass(i);
   };
   auto radix_call = [arr, sz, mem1, mem2]() {
       return RadixSort(arr.get(), sz, SomeClass::getKey, mem1, mem2);
   };
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, sz]() { check(arr.get(), arr_ok.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);

   delete [] mem1; mem1 = nullptr;
   free(mem2); mem2 = nullptr;
}

void TestArrUserDefinedTypeAllocHerePartially(size_t sz)
{
   cout << "\nSorting array of " << sz << " class objects, allocation of helper memory partially by client\n";

   auto arr = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto arr_ok = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto mem1 = std::shared_ptr<size_t[]>(new size_t[sz * 2]);

   auto create_entry = [](SomeClass *elem1, SomeClass *elem2, size_t i) {
       *elem1 = *elem2 = SomeClass(i);
   };
   auto radix_call = [arr, sz, mem1]() {
       return RadixSort(arr.get(), sz, SomeClass::getKey, mem1.get());};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, sz]() { check(arr.get(), arr_ok.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

void TestArrUserDefinedTypeIndexesOnly(size_t sz)
{
   cout << "\nSorting array of " << sz << " class objects, indexes only.\n";

   auto arr = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto arr_ok = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto idxs = std::shared_ptr<size_t[]>(new size_t[sz]);

   auto create_entry = [](SomeClass *elem1, SomeClass *elem2, size_t i) {
       *elem1 = *elem2 = SomeClass(i);
   };
   auto radix_call = [arr, sz, idxs]() {
       return RadixSortIndexesOnly(arr.get(), sz, SomeClass::getKey, idxs.get());};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, idxs, sz]() { check(arr.get(), arr_ok.get(), idxs.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

void TestArrUserDefinedTypeIndexesOnlyAllocHere(size_t sz)
{
   cout << "\nSorting array of " << sz << " class objects, indexes only, allocation of helper memory by client.\n";

   auto arr = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto arr_ok = std::shared_ptr<SomeClass[]>(new SomeClass[sz]);
   auto idxs = std::shared_ptr<size_t[]>(new size_t[sz]);
   auto mem1 = std::shared_ptr<size_t[]>(new size_t[sz * 2]);
   auto mem2 = std::shared_ptr<size_t[]>(new size_t[sz * 2]);

   auto create_entry = [](SomeClass *elem1, SomeClass *elem2, size_t i) {
       *elem1 = *elem2 = SomeClass(i);
   };
   auto radix_call = [arr, sz, idxs, mem1, mem2]() {
       return RadixSortIndexesOnly(arr.get(), sz, SomeClass::getKey, idxs.get(), mem1.get(), mem2.get());};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, idxs, sz]() { check(arr.get(), arr_ok.get(), idxs.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

template <class T>
void TestArrIntegralType(
       size_t max_val,
       size_t sz,
       bool all_positive,
       bool all_negative = false)
{
   cout << "\nSorting array of " << sz << " " << typeid(T).name();
   if (all_positive) cout << ", all positive\n";
   else if (all_negative) cout << ", all negative\n";
   else cout << ", positive and negative\n";

   auto arr = std::shared_ptr<T[]>(new T[sz]);
   auto arr_ok = std::shared_ptr<T[]>(new T[sz]);

   auto create_entry = [max_val, all_positive, all_negative]
                       (T *elem1, T *elem2, size_t) {
       *elem1 = *elem2 = GetRandIntegral<T>(max_val, all_positive, all_negative);
   };
   auto radix_call = [arr, sz]() {return RadixSort(arr.get(), sz);};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, sz]() { check(arr.get(), arr_ok.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

template <class T>
void TestArrIntegralTypeAllocHere(
       size_t max_val,
       size_t sz,
       bool all_positive,
       bool all_negative = false)
{
   cout << "\nSorting array of " << sz << " integral type " << typeid(T).name();
   if (all_positive) cout << ", all positive";
   else if (all_negative) cout << ", all negative";
   else cout << ", positive and negative";
   cout << ", allocation of helper memory by client\n";

   auto arr = std::shared_ptr<T[]>(new T[sz]);
   auto arr_ok = std::shared_ptr<T[]>(new T[sz]);
   auto mem = std::shared_ptr<char[]>(new char[sz * sizeof(T)]);

   auto create_entry = [max_val, all_positive, all_negative]
                       (T *elem1, T *elem2, size_t) {
       *elem1 = *elem2 = GetRandIntegral<T>(max_val, all_positive, all_negative);
   };
   auto radix_call = [arr, sz, mem]() {return RadixSort(arr.get(), sz, mem.get());};
   auto std_call = [arr_ok, sz](){std::sort(arr_ok.get(), arr_ok.get() + sz);};
   auto check_call = [arr, arr_ok, sz]() { check(arr.get(), arr_ok.get(), sz); };

   TestImpl(arr.get(), arr_ok.get(), sz, create_entry, radix_call, std_call, check_call);
}

void TestListUserDefinedType(size_t sz)
{
   cout << "\nSorting std::list of " << sz << " class objects\n";

   list<SomeClass> lst, lst_ok;

   using it_t = list<SomeClass>::iterator;
   auto create_entry = [&lst, &lst_ok](it_t, it_t, size_t i) {
       SomeClass f(i);
       try {
           lst.push_back(f);
           lst_ok.push_back(f);
       }
       catch(exception &e) {
           cout << "Caught during std::list<SomeClass>::push_back():\n" << e.what() << endl;
       }
   };
   auto radix_call = [&lst]() {return RadixSort(lst, SomeClass::getKey);};
   auto std_call = [&lst_ok](){lst_ok.sort();};
   auto check_call = [&lst, &lst_ok, sz]() {check(lst, lst_ok, sz);};

   TestImpl(lst.begin(), lst_ok.begin(), sz, create_entry, radix_call, std_call, check_call);
}

void TestListUserDefinedTypeAllocateHere(size_t sz)
{
   cout << "\nSorting std::list of " << sz << " class objects\n";

   list<SomeClass> lst, lst_ok;
   const size_t mem_sz = (sz * (sizeof(size_t) + sizeof(list<SomeClass>::const_iterator)));
   auto mem1 = shared_ptr<char[]>(new char[mem_sz]);
   auto mem2 = shared_ptr<char[]>(new char[mem_sz]);

   using it_t = list<SomeClass>::iterator;
   auto create_entry = [&lst, &lst_ok](it_t, it_t, size_t i) {
       SomeClass f(i);
       try {
           lst_ok.push_back(f);
           lst.push_back(f);
       }
       catch(exception &e) {
           cout << "Caught during std::list<SomeClass>::push_back():\n" << e.what() << endl;
       }
   };
   auto radix_call = [&lst, mem1, mem2]() {return RadixSort(lst, SomeClass::getKey, mem1.get(), mem2.get());};
   auto std_call = [&lst_ok](){lst_ok.sort();};
   auto check_call = [&lst, &lst_ok, sz]() {check(lst, lst_ok, sz);};

   TestImpl(lst.begin(), lst_ok.begin(), sz, create_entry, radix_call, std_call, check_call);
}

void TestVectorIntegralType(size_t sz)
{
   cout << "\nSorting vector of " << sz << " unsigned int\n";

   vector<unsigned int> vec(sz), vec_ok(sz);

   using it_t = vector<unsigned int>::iterator;
   auto create_entry = [](it_t elem1, it_t elem2, size_t) {
       *elem1 = *elem2 = GetRandIntegral<unsigned int>(UINT_MAX, false);
   };
   auto radix_call = [&vec, sz]() {return RadixSort(&(vec[0]), sz);};
   auto std_call = [&vec_ok](){std::sort(vec_ok.begin(), vec_ok.end());};
   auto check_call = [&vec, &vec_ok, sz]() { check(vec.begin(), vec_ok.begin(), sz); };

   TestImpl(vec.begin(), vec_ok.begin(), sz, create_entry, radix_call, std_call, check_call);
}

void TestVectorUserDefinedType(size_t sz)
{
   cout << "\nSorting vector of " << sz << " class objects\n";

   vector<SomeClass> vec;
   vector<SomeClass> vec_ok;

   using it_t = vector<SomeClass>::iterator;
   auto create_entry = [&vec, &vec_ok](it_t, it_t, size_t i) {
       SomeClass f(i);
       try {
           vec.push_back(f);
           vec_ok.push_back(f);
       }
       catch(exception &e) {
           cout << "Caught during std::vector<SomeClass>::push_back():\n" << e.what() << endl;
       }
   };
   auto radix_call = [&vec, sz]() {return RadixSort(&(vec[0]), sz, SomeClass::getKey);};
   auto std_call = [&vec_ok](){std::sort(vec_ok.begin(), vec_ok.end());};
   auto check_call = [&vec, &vec_ok, sz]() { check(vec.begin(), vec_ok.begin(), sz); };

   TestImpl(vec.begin(), vec_ok.begin(), sz, create_entry, radix_call, std_call, check_call);
}

int main()
{
   const unsigned int sz = 10000000;

   srand(time(0));

   TestArrIntegralTypeAllocHere<size_t>(SIZE_MAX, sz, false);
   TestArrIntegralType<int>(INT_MAX, sz, false, true);
    TestArrIntegralType<int>(INT_MAX, sz, true);
   TestArrIntegralType<int>(INT_MAX, sz - 1, false);
   TestArrIntegralType<int>(INT_MAX, sz, false);
   TestArrIntegralType<int>(INT_MAX, sz, false);
   TestArrIntegralType<short>(SHRT_MAX, sz, false);

   TestListUserDefinedType(sz);
   TestListUserDefinedTypeAllocateHere(sz);

   TestArrUserDefinedType(sz);
   TestArrUserDefinedTypeAllocHere(sz);
   TestArrUserDefinedTypeAllocHerePartially(sz);

   TestArrUserDefinedTypeIndexesOnly(sz);
   TestArrUserDefinedTypeIndexesOnlyAllocHere(sz);

   TestVectorIntegralType(sz);
   TestVectorUserDefinedType(sz);

//    TestListUserDefinedType(1);
//    TestListUserDefinedType(0);
//    TestArrUserDefinedType(1);
//    TestArrUserDefinedTypeIndexesOnly(1);

   return (0);
}



