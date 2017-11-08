
#ifndef ARGUMENT_MAP_HPP_
#define ARGUMENT_MAP_HPP_

#include <string>
#include <vector>
#include <stdexcept>
#include <map>

#include "Argument_type.hpp"
#include "Argument_range.hpp"
#include "Argument_type_basics.hpp"
#include "Argument_range_basics.hpp"

namespace aff3ct
{
namespace tools
{

using Argument_tag = std::vector<std::string>;

struct Argument_info
{
    Argument_info()
    {}

    Argument_info(Argument_type* type, std::string doc)
    : type(type), doc(doc)
    {}

	virtual Argument_info clone() const
	{
		Argument_type* arg_t = nullptr;

		if (type != nullptr)
			arg_t = type->clone();

		return Argument_info(arg_t, doc);
	}

	Argument_type* type = nullptr;
	std::string    doc  = "";
};


class Argument_map_info : public std::map<Argument_tag, Argument_info>
{
public:
	using mother = std::map<Argument_tag, Argument_info>;

public:
	Argument_map_info()
	{ }

	Argument_map_info(const Argument_map_info& other)
	{
		*this = other.clone();
	}

	virtual ~Argument_map_info()
	{
		clear();
	}

	void add(const Argument_tag& tags, Argument_type* arg_t, const std::string& doc)
	{
		if (tags.size() == 0)
			throw std::invalid_argument("No tag has been given ('tag.size()' == 0).");

		if (arg_t == nullptr)
			throw std::invalid_argument("No argument type has been given ('arg_t' == 0).");

		(*this)[tags];
		(*this)[tags].type = arg_t;
		(*this)[tags].doc  = doc;
	}

	void clear()
	{
		for (auto it = this->begin(); it != this->end(); it++)
			if (it->second.type != nullptr)
				delete it->second.type;

		mother::clear();
	}

	Argument_map_info clone() const
	{
		Argument_map_info other;

		for (auto it = this->begin(); it != this->end(); it++)
			other[it->first] = it->second.clone();

		return other;
	}

	bool exist(const Argument_tag &tags)
	{
		return (this->find(tags) != this->end());
	}
};


class Argument_map_value : public std::map<Argument_tag, std::string>
{
public:
	using mother = std::map<Argument_tag, std::string>;

public:

	bool exist(const Argument_tag &tags) const
	{
		return (this->find(tags) != this->end());
	}

	/*!
	 * \brief Returns the value for an argument.
	 *
	 * \param tags: list of tags associated to an argument, tags = {"Key1", "Key2", [...]}
	 *
	 * \return the integer value of an argument with its tags (to use after the parse_arguments method).
	 */
	int to_int(const Argument_tag &tags) const
	{
		try
		{
			return std::stoi(this->at(tags));
		}
		catch (std::exception&)
		{
			return 0;
		}
	}

	/*!
	 * \brief Returns the value for an argument.
	 *
	 * \param tags: list of tags associated to an argument, tags = {"Key1", "Key2", [...]}.
	 *
	 * \return the floating-point value of an argument with its tags (to use after the parse_arguments method).
	 */
	float to_float(const Argument_tag &tags) const
	{
		try
		{
			return std::stof(this->at(tags));
		}
		catch (std::exception&)
		{
			return 0.0f;
		}
	}

};


class Argument_map_group : public std::map<std::string, std::string>
{
public:
	using mother = std::map<std::string, std::string>;

public:

	bool exist(const std::string &prefix) const
	{
		return (this->find(prefix) != this->end());
	}

	void add(const std::string& prefix, const std::string& title)
	{
		if (prefix.size() == 0)
			throw std::invalid_argument("No prefix has been given ('prefix.size()' == 0).");

		if (title.size() == 0)
			throw std::invalid_argument("No title has been given ('title.size()' == 0).");

		(*this)[prefix] = title;
	}

};

}
}

#endif /* ARGUMENT_MAP_HPP_ */
