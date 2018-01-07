#ifndef ARGUMENT_TYPE_LIST_HPP_
#define ARGUMENT_TYPE_LIST_HPP_

#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <tuple>

#include "Argument_type.hpp"
#include "../Splitter/Splitter.hpp"

namespace aff3ct
{
namespace tools
{

template <typename List_element_type = std::string, class S = Generic_splitter, typename... Ranges>
class List_type : public Argument_type_limited<std::vector<List_element_type>,Ranges...>
{
protected:
	Argument_type* val_type;

public:
	List_type(Argument_type* val_type, const Ranges*... ranges)
	: Argument_type_limited<std::vector<List_element_type>,Ranges...>("list of " + val_type->get_short_title(), ranges...), val_type(val_type)
	{ }

	virtual ~List_type()
	{
		if (val_type != nullptr) delete val_type;
	};

	virtual List_type<List_element_type, S, Ranges...>* clone() const
	{
		auto clone = new List_type<List_element_type, S, Ranges...>(*this);

		clone->val_type = val_type->clone();

		return dynamic_cast<List_type<List_element_type, S, Ranges...>*>(this->clone_ranges(clone));
	}

	virtual void check(const std::string& val) const
	{
		// separate values:
		auto list = S::split(val);

		unsigned i = 0;
		try
		{
			for (; i < list.size(); i++)
				val_type->check(list[i]);
		}
		catch(std::exception& e)
		{
			std::stringstream message;
			message << "has the element " << i << " (" << list[i] << ") not respecting the rules: " << e.what();

			throw std::runtime_error(message.str());
		}

		auto list_vals = this->convert(list);

		this->check_ranges(list_vals);

	}

	virtual const std::string get_title() const
	{
		auto t = "list of (" + val_type->get_title() + ")";

		if (sizeof...(Ranges)) // then add ranges titles to the argument title
		{
			t += Argument_type::title_description_separator;
			this->get_ranges_title(t);
		}

		return t;
	}

	virtual std::vector<List_element_type> convert(const std::string& val) const
	{
		return this->convert(S::split(val));
	}

	virtual std::vector<List_element_type> convert(const std::vector<std::string>& list) const
	{
		std::vector<List_element_type> list_T(list.size());

		void * p_val = nullptr;

		for(unsigned i = 0; i < list.size(); i++)
		{
			p_val = val_type->get_val(list[i]);

			if (p_val == nullptr)
				throw std::runtime_error("Couldn't convert value.");

			list_T[i] = *(List_element_type*)p_val;

			delete (List_element_type*)p_val;
		}

		return list_T;
	}
};

template <typename T = std::string, class S = Generic_splitter, typename... Ranges>
List_type<T,S,Ranges...>* List(Argument_type* val_type, Ranges*... ranges)
{
	return new List_type<T,S,Ranges...>(val_type, ranges...);
}

template<int...> struct index_tuple{};

template<int I, typename IndexTuple, typename... Types>
struct make_indexes_impl;

template<int I, int... Indexes, typename T, typename ... Types>
struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...>
{
    typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
};

template<int I, int... Indexes>
struct make_indexes_impl<I, index_tuple<Indexes...> >
{
    typedef index_tuple<Indexes...> type;
};

template<typename ... Types>
struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...>
{};


template<class Ret, class... Args, int... Indexes >
Ret apply_helper( Ret (*pf)(Args...), index_tuple< Indexes... >, std::tuple<Args...>&& tup)
{
    return pf( std::forward<Args>( std::get<Indexes>(tup))... );
}

template<class Ret, class ... Args>
Ret apply(Ret (*pf)(Args...), const std::tuple<Args...>&  tup)
{
    return apply_helper(pf, typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
}

template<class Ret, class ... Args>
Ret apply(Ret (*pf)(Args...), std::tuple<Args...>&&  tup)
{
    return apply_helper(pf, typename make_indexes<Args...>::type(), std::forward<std::tuple<Args...>>(tup));
}



template <typename T = std::string,
          class S1 = Generic_splitter, class S2 = String_splitter,
          typename... Ranges1, typename... Ranges2>
List_type<std::vector<T>,S1,Ranges1...>* List2D(Argument_type* val_type, std::tuple<Ranges1*...>&& ranges1, std::tuple<Ranges2*...>&& ranges2)
{
	Argument_type* listD2 = apply(List<T,S2,Ranges2...>, std::tuple_cat(std::make_tuple(val_type), ranges2));
	return apply(List<std::vector<T>,S1,Ranges1...>, std::tuple_cat(std::make_tuple(listD2), ranges1));
}

}
}
#endif /* ARGUMENT_TYPE_LIST_HPP_ */