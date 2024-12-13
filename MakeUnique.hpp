/**
 @file MakeUnique.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_MakeUnique_hpp
#define Chaos_MakeUnique_hpp

#include <future>

namespace chaos {
	/**
	 @brief Недостающий аналог std::make_shared<>
	 @details Exception-safe метод создания умных указателей
	 
	 @param ...args Аргументы для конструктора
	 
	 @return Умный уникальный указатель
	 */
	template<typename T, typename ...TArgs>
	std::unique_ptr<T> make_unique(TArgs&& ...args)
	{
		return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
	}
}

#endif
