#ifndef RADIX_SORT_INTERNAL
#define RADIX_SORT_INTERNAL

#include <list>
#include <memory>
#include <climits>

template <class T>
auto GetMem(size_t sz, void *usable_mem = nullptr)
{
    using mem_ptr = std::unique_ptr<T[], void(*)(T*)>;

    return (nullptr == usable_mem) ?
            mem_ptr(new T[sz], [](T* mem){delete [] mem;}) :
            mem_ptr((T*)usable_mem, [](T*){});
}

template<class T>
void CountingIntegral(
        const T *arr,
        size_t   arr_sz,
        size_t   cur_byte_idx,
        T       *out_arr)
{
    size_t histogram[256] = {0};
    size_t shift_bits = (cur_byte_idx - 1) * 8;
    const size_t mask = ~(size_t)0 >> ((sizeof(size_t) - cur_byte_idx) * 8);

    for (size_t i = 0; i < arr_sz; ++i) {
        ++histogram[(arr[i] & mask) >> shift_bits];
    }

    for (size_t i = 0, offset = 0; i < 256; ++i) {
        size_t bucket_sz = histogram[i];
        histogram[i] = offset;
        offset += bucket_sz;
    }

    for (size_t i = 0; i < arr_sz; ++i) {
        size_t idx = (arr[i] & mask) >> shift_bits;
        out_arr[histogram[idx]] = arr[i];
        ++histogram[idx];
    }
}

template<class T>
void RadixIntegral(T *arr, size_t sz, void *helper_arr = nullptr)
{
    // 1. rearrange arr: min values of type T, then negative values, then positive values.
    // 2. turn only the part of the negative values into positive values.
    // 3. sort the negative part separately and the positive part separately.
    // 4. turn the values in the negative part back to negative, and reverse them.

    T *pos_begin = arr;
    for (T *p = arr, *p_end = arr + sz; p < p_end; ++p) {
        if (*p < 0) {
            std::swap(*pos_begin, *p);
            ++pos_begin;
        }
    }

    T t_min = std::numeric_limits<T>::min();
    T *neg_begin = arr;
    for (T *p = arr; p < pos_begin; ++p) {
        if (t_min == *p) {
            std::swap(*neg_begin, *p);
            ++neg_begin;
        }
        else {
            *p = -*p;
        }
    }

    auto out_arr = GetMem<T>(sz, helper_arr);
    for (auto [round, out_neg_ptr, out_pos_ptr, pos_sz, neg_sz] = std::tuple{
                1U,
                out_arr.get() + (neg_begin - arr),
                out_arr.get() + (pos_begin - arr),
                sz - (pos_begin - arr),
                pos_begin - neg_begin};
        round <= sizeof(T);
        ++round) {
            CountingIntegral(neg_begin, neg_sz, round, out_neg_ptr);
            CountingIntegral(pos_begin, pos_sz, round, out_pos_ptr);

            std::swap(neg_begin, out_neg_ptr);
            std::swap(pos_begin, out_pos_ptr);
    }

    for (T *neg_rev = pos_begin - 1; neg_begin <= neg_rev; ++neg_begin, --neg_rev) {
        T tmp = -*neg_begin;
        *neg_begin = -*neg_rev;
        *neg_rev = tmp;
    }
}

template<class LOC>
struct RadixEntry
{
    // idx/iterator to element in original container.
    LOC     m_first;
    // two functionalities:
    // when sorting: representation of the element as unsigned integral type.
    // when rearranging consecutive memory: an index, for keeping track of the
    // moving elements. This is why the field is size_t.
    size_t  m_second;
};

template <class LOCATION_TYPE>
void CountingUserDefined(
        const RadixEntry<LOCATION_TYPE>  *arr,
        size_t                            arr_sz,
        size_t                            round,
        RadixEntry<LOCATION_TYPE>        *out)
{
    size_t histogram[256] = {0};
    const size_t shift_bits = (round - 1) * 8;
    const size_t mask = ~(size_t)0 >> ((sizeof(size_t) - round) * 8);

    for (size_t i = 0; i < arr_sz; ++i) {
        ++histogram[(arr[i].m_second & mask) >> shift_bits];
    }

    for (size_t i = 0, offset = 0; i < 256; ++i) {
        size_t bucket_sz = histogram[i];
        histogram[i] = offset;
        offset += bucket_sz;
    }

    for (size_t i = 0; i < arr_sz; ++i) {
        size_t idx = (arr[i].m_second & mask) >> shift_bits;
        out[histogram[idx]] = arr[i];
        ++histogram[idx];
    }
}

template<class T>
void RearrangeArr(
        T                           *arr,
        size_t                       arr_sz,
        const RadixEntry<size_t>    *sorted,
        RadixEntry<size_t>          *locs)
{
    // locs will keep track of where each element from the original array is,
    // as the elements swap places during the in-place re-ordering.
    // The members m_first and m_second will function as follows:
    // m_first: current idx of residence of the element that originally resided here.
    // m_second: original idx of residence of the element that currently resides here.

    for (size_t i = 0; i < arr_sz; ++i) {
        locs[i].m_first = locs[i].m_second = i;
    }

    for (size_t i = 0; i < arr_sz - 1; ++i) {
        size_t where_is_elem = locs[sorted[i].m_first].m_first;

        if (i != where_is_elem) {
            locs[where_is_elem].m_second = locs[i].m_second;
            locs[locs[i].m_second].m_first = where_is_elem;

            T tmp = std::move(arr[i]);
            arr[i] = std::move(arr[where_is_elem]);
            arr[where_is_elem] = std::move(tmp);
        }
    }
}

template <class U, class LOCATION_TYPE, class T_ITERAROT, class INIT_FUNC>
void RadixImpl(
        T_ITERAROT                   it,
        size_t                       sz,
        RadixEntry<LOCATION_TYPE>   *sorted,
        RadixEntry<LOCATION_TYPE>   *to_sort,
        const INIT_FUNC             &init_radix_entry)
{
    for (size_t i = 0; i < sz; ++it, ++i) {
        init_radix_entry(sorted[i], it, i);
    }

    for (size_t round = 1, rounds = std::max(size_t(2), sizeof(U)); round <= rounds; ++round) {
        CountingUserDefined(sorted, sz, round, to_sort);
        std::swap(to_sort, sorted);
    }
}

template <class U, class T, class PREPARE_OUTPUT>
void RadixConsecutive(
        const T                 *arr,
        size_t                   arr_sz,
        U                       (T_to_unsigned)(const T&),
        PREPARE_OUTPUT           prepare_output,
        void                    *usable_mem1 = nullptr,
        void                    *usable_mem2 = nullptr)
{
    using radix_entry_t = RadixEntry<size_t>;

    auto sorted = GetMem<radix_entry_t>(arr_sz, usable_mem1);
    auto to_sort = GetMem<radix_entry_t>(arr_sz, usable_mem2);
    auto init_radix_entry =
        [&T_to_unsigned](radix_entry_t &entry, const T *it_to_elem, size_t elem_idx){
            entry.m_first = elem_idx;
            entry.m_second = T_to_unsigned(*it_to_elem);
    };
    RadixImpl<U>(
        arr,
        arr_sz,
        sorted.get(),
        to_sort.get(),
        init_radix_entry);

    prepare_output(sorted.get(), to_sort.get());
}

template<class U, class T, typename it_t = typename std::list<T>::const_iterator>
void RearrangeList(std::list<T> &lst, size_t lst_sz, RadixEntry<it_t> *sorted)
{
    auto *it_sorted = sorted;
    auto *sorted_last = sorted + lst_sz - 1;

    while (it_sorted < sorted_last){
        it_t splice_begin = it_sorted->m_first;
        it_t splice_end = splice_begin; ++splice_end;

        // splice as large chunks as possible
        while ((it_sorted < sorted_last) && (it_sorted[1].m_first == splice_end)) {
            ++it_sorted;
            ++splice_end;
        }

        lst.splice(lst.end(), lst, splice_begin, splice_end);
        ++it_sorted;
    }
    it_t splice_end = sorted_last->m_first; ++splice_end;
    lst.splice(lst.end(), lst, sorted_last->m_first, splice_end);
}

template <class T, class U, typename it_t = typename std::list<T>::const_iterator>
void RadixListImpl(
        std::list<T>            &lst,
        U                       (T_to_unsigned)(const T&),
        void                    *usable_mem1 = nullptr,
        void                    *usable_mem2 = nullptr)
{
    using radix_entry_t = RadixEntry<it_t>;

    size_t lst_sz = lst.size();
    auto sorted = GetMem<radix_entry_t>(lst_sz, usable_mem1);
    auto to_sort = GetMem<radix_entry_t>(lst_sz, usable_mem2);

    auto init_radix_entry = [&T_to_unsigned]
            (radix_entry_t &entry, it_t it, size_t) {
                entry.m_first = it;
                entry.m_second = T_to_unsigned(*it);
            };
    RadixImpl<U>(
        lst.begin(),
        lst_sz,
        sorted.get(),
        to_sort.get(),
        init_radix_entry);

    RearrangeList<U>(lst, lst_sz, sorted.get());
}

#endif // RADIX_SORT_INTERNAL
