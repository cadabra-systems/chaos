#ifndef Chaos_Globus_L10N_hpp
#define Chaos_Globus_L10N_hpp

#include "Globus.hpp"

#include <unicode/ures.h>

namespace chaos { namespace globus {
	class l10n
	{
	/** @name Statics */
	/** @{ */
	public:
		static std::shared_ptr<l10n> instance(const std::string& resource_path, const std::string& class_name, const std::string& locale);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		l10n(const std::string& resource_path, const std::string& extension_name, const std::string& locale, const std::string& resource_table_key);
		~l10n();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _extension_name;
		std::string _locale;
		std::string _resource_table_key;

		UResourceBundle* _resource_table;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		std::pair<std::string, bool> operator[](const std::string& key) const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_extension_name() const;
		const std::string& get_locale() const;
		const std::string& get_resource_table_key() const;
	/** @} */
	};
} }

namespace std {
	template<>
	class hash<shared_ptr<chaos::globus::l10n>>
	{
	public:
		size_t operator()(const shared_ptr<chaos::globus::l10n>& localization) const;
	};
}
#endif
