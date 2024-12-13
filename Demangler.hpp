/**
 @file Delegate.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 @todo Закончить с комментариями
 */

#ifndef Chaos_Demangler_hpp
#define Chaos_Demangler_hpp

#include <string>
#include <memory>
#include <cstdlib>
#include <typeinfo>
#include <cxxabi.h>

namespace chaos {
	class demangler
	{
	/** @name Static */
	/** @{ */
	public:
		static std::string name(const std::type_info& type_info)
		{
			int status(-4);
			size_t length(0);
			std::unique_ptr<char, void(*)(void*)> retval {
															abi::__cxa_demangle(type_info.name(), nullptr, &length, &status),
															std::free
			};
			return status == 0 && length > 0 ? std::string(retval.get(), length - 1) : std::string(type_info.name());
		}

		static std::string name(const char* type_name)
		{
			int status(-4);
			size_t length(0);
			std::unique_ptr<char, void(*)(void*)> retval {
															abi::__cxa_demangle(type_name, nullptr, &length, &status),
															std::free
			};
			return status == 0 && length > 0 ? std::string(retval.get(), length - 1) : type_name;
		}
		
		static bool compare(const std::type_info& type_info, const std::string& rhs_name)
		{
			/// @??? case?!
			return rhs_name.compare(demangler::name(type_info)) == 0;
		}

		template <typename T>
		static std::enable_if<std::is_pointer<T>::value, const std::type_info&>
		type_info(T target)
		{
			return typeid(*target);
		}

		template <typename T>
		static const std::type_info& type_info(const std::shared_ptr<T>& target)
		{
			T* raw_pointer(target.get());
			return typeid(raw_pointer);
		}

		template <typename T>
		static const std::type_info& type_info(const std::unique_ptr<T>& target)
		{
			T* raw_pointer(target.get());
			return typeid(raw_pointer);
		}
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		demangler() = delete;
		~demangler() = default;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}
#endif

