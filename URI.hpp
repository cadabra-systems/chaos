/**
 @file URI.hpp
 @date 04.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_URI_hpp
#define Chaos_URI_hpp

#include "String.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <type_traits>
#include <cstdlib>

namespace chaos {
	class uri
	{
	/** @name Constants */
	/** @{ */
	public:
		enum class subset : char
		{
			locator = 'L',
			name = 'N'
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		uri(const std::string& in = "")
		{
			if (in.length() > 0) {
				parse(std::move(in));
			} else {
				_string = "";
				_scheme = "";
				_host = "";
				_path = "";
				_query = "";
				_password = "";
				_username = "";
				
				_port = 0;
			}
		}

		uri(const uri& origin)
		:
			_string(origin._string),
			_scheme(origin._scheme),
			_host(origin._host),
			_path(origin._path),
			_query(origin._query),
			_username(origin._username),
			_password(origin._password),
			_port(origin._port)
		{

		}

		uri(uri&& origin)
		:
			_string(std::move(origin._string)),
			_scheme(std::move(origin._scheme)),
			_host(std::move(origin._host)),
			_path(std::move(origin._path)),
			_query(std::move(origin._query)),
			_username(std::move(origin._username)),
			_password(std::move(origin._password)),
			_port(std::move(origin._port))
		{

		}
		
		~uri() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		uri& operator=(const uri& rhs)
		{
			_string = rhs._string;
			_scheme = rhs._scheme;
			_host = rhs._host;
			_path = rhs._path;
			_query = rhs._query;
			_username = rhs._username;
			_password = rhs._password;
			_port = rhs._port;

			return *this;
		}

		uri& operator=(uri&& rhs)
		{
			_string = std::move(rhs._string);
			_scheme = std::move(rhs._scheme);
			_host = std::move(rhs._host);
			_path = std::move(rhs._path);
			_query = std::move(rhs._query);
			_username = std::move(rhs._username);
			_password = std::move(rhs._password);
			_port = std::move(rhs._port);

			return *this;
		}

		bool operator<(const uri& rhs) const
		{
			return _string < rhs._string;
		}

		uri& operator<<(std::istream& stream)
		{
			/// @todo нестандартный парсинг
			/// без URL decoding
			/// с умной остановкой считывания: до whitespace но если есть открытый {([, то продолжать до закрытия или до конца строки
			std::string uri;
			std::getline(stream, uri);
			parse(uri);

			return *this;
		}

		bool operator==(const uri& rhs) const
		{
			return rhs._string == _string;
		}

		friend void swap(uri& a, uri& b)
		{
			std::swap(a._string, b._string);
			std::swap(a._scheme, b._scheme);
			std::swap(a._host, b._host);
			std::swap(a._path, b._path);
			std::swap(a._query, b._query);
			std::swap(a._username, b._username);
			std::swap(a._password, b._password);
			std::swap(a._port, b._port);
		}
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::string _string;
		
		std::string _scheme;
		std::string _host;
		std::string _path;
		std::string _query;
		std::string _username;
		std::string _password;

		unsigned short _port;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		std::string parse(const std::string& in)
		{
			_string = string::trimmed_string(in);

			std::string temp(_string);
			
			_scheme = string::slice_head_string(temp, "://");
			_query = string::slice_tail_string(temp, "?", false);
			_path = string::slice_tail_string(temp, "/", false);
			
			std::string credentials = string::slice_head_string(temp, temp.find_last_of("@"), 1);
			_password = string::slice_tail_string(credentials, ":");
			_username = credentials;
			
			_port = static_cast<unsigned short>(std::atoi(string::slice_tail_string(temp, ":").c_str()));
			_host = temp;
			
			return temp;
		}

		void clear()
		{
			_string.clear();
			_scheme.clear();
			_host.clear();
			_path.clear();
			_query.clear();
			_password.clear();
			_username.clear();
			_port = 0;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		operator const std::string&() const
		{
			return _string;
		}

		operator std::string() const
		{
			return _string;
		}

		const std::string& string() const
		{
			return _string;
		}
		
		const std::string& scheme() const
		{
			return _scheme;
		}
		
		const std::string& host() const
		{
			return _host;
		}
		
		const std::string& path() const
		{
			return _path;
		}
		
		const std::string& query() const
		{
			return _query;
		}

		template <typename I>
		typename std::enable_if<std::is_integral<I>::value, I>::type
		parameter(const std::string& name, const I& default_value) const
		{
			try {
				if constexpr (std::is_same<int, I>::value) {
					return std::stoi(parameter(name, std::to_string(default_value)));
				} else if constexpr (std::is_same<long, I>::value) {
					return std::stol(parameter(name, std::to_string(default_value)));
				} else if constexpr (std::is_same<unsigned long, I>::value) {
					return std::stoul(parameter(name, std::to_string(default_value)));
				} else if constexpr (std::is_same<long long, I>::value) {
					return std::stoll(parameter(name, std::to_string(default_value)));
				} else if constexpr (std::is_same<unsigned long long, I>::value) {
					return std::stoull(parameter(name, std::to_string(default_value)));
				}
			} catch (...) {
				return default_value;
			}
			return default_value;
		}

		template <typename S>
		typename std::enable_if<std::is_same<S, std::string>::value, S>::type
		parameter(const std::string& name, const S& default_value) const
		{
			std::string::size_type offset = _query.find(name + "=");
			if (std::string::npos == offset) {
				return default_value;
			}
			offset += name.size() + 1;
			std::string::const_iterator i = std::find_if
			(
				_query.begin() + offset, _query.end(),
				[](char c)
				{
					return c == '&' || c == ';' || c == '#';
				}
			);
			if (i == _query.end()) {
				return _query.substr(offset);
			}
			return _query.substr(offset, std::distance(_query.begin() + offset, i));
		}

		const std::string& username() const
		{
			return _username;
		}
		
		const std::string& password() const
		{
			return _password;
		}
		
		const unsigned short& port() const
		{
			return _port;
		}

		std::size_t hash() const
		{
			return std::hash<std::string>()(_string);
		}

		uri::subset subset() const
		{
			/// @todo
			return subset::locator;
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void set_parameter(const std::string& name, const std::string& value = "")
		{
			std::string::size_type offset = _query.find(name + "=");
			if (std::string::npos == offset) {
				if (_query.empty()) {
					_query.append(name + "=" + value);
				} else {
					_query.append("&" + name + "=" + value);
				}
			}

			offset += name.size() + 1;
			std::string::const_iterator i(
											std::find_if(
														 _query.begin() + offset, _query.end(),
														 [](char c)
														 {
															 return c == '&' || c == ';' || c == '#';
														 }
											)
			);
			_query.replace(_query.begin() + offset, i, value);
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool valid() const
		{
			return
					!_scheme.empty()
					&&
					(
						(
							_scheme.compare("file") == 0
							&&
							!_path.empty()
						)
						||
						!_host.empty()
					)
			;
		}
	/** @} */
	};
}

#endif
