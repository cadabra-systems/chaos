#include "L10N.hpp"

#include "../String.hpp"
#include "../AtomicHashTable.hpp"

#include "../Log.hpp"
#include "../LogRegister.hpp"

#include <unicode/ustring.h>

namespace chaos { namespace globus {
	std::shared_ptr<l10n> l10n::instance(const std::string& resource_path, const std::string& class_name, const std::string& locale)
	{
		static chaos::atomic_hash_table<std::shared_ptr<l10n>> instance;
		chaos::atomic_hash_table<std::shared_ptr<l10n>>::const_iterator i(instance.end());
		bool ok(false);

		const std::vector<std::string> namespaces(chaos::string::explode<std::vector, std::string>(class_name, "::"));

		if (namespaces.size() <= 3) {
			chaos::log_register<chaos::log>::warning("l10n::instance() > Invalid class name is specified");
			return std::shared_ptr<l10n>(nullptr);
		}

		const std::string extension_name(namespaces.at(2));
		std::vector<std::string>::const_iterator it(namespaces.begin());
		std::advance(it, 3);

		const std::string resource_table_key(chaos::string::join<std::vector, std::string>(it, namespaces.end(), "_"));
		std::tie(i, ok) = instance.try_emplace(std::make_shared<l10n>(resource_path, extension_name, locale, resource_table_key));

		if (!ok) {
			i = instance.find(std::hash<std::string>()(extension_name + locale + resource_table_key));

			if (i == instance.cend()) {
				chaos::log_register<chaos::log>::warning("l10n::instance() > Something is wrong with chaos::atomic_hash_table");
				return std::shared_ptr<l10n>(nullptr);
			}
		}

		return i->second;
	}

	l10n::l10n(const std::string& resource_path, const std::string& extension_name, const std::string& locale, const std::string& resource_table_key)
	:
		_extension_name(extension_name),
		_locale(locale),
		_resource_table_key(resource_table_key),
		_resource_table(nullptr)
	{
		const std::string bundle_path(resource_path + "/" + _extension_name);

		UErrorCode status(U_ZERO_ERROR);
		UResourceBundle* resource_bundle(ures_open(bundle_path.c_str(), locale.c_str(), &status));

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("l10n::l10n() > ", u_errorName(status));
			return ;
		}

		_resource_table = ures_getByKey(resource_bundle, _resource_table_key.c_str(), NULL, &status);

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("l10n::l10n() > ", u_errorName(status));
			_resource_table = nullptr;
			return ;
		}

		ures_close(resource_bundle);
	}

	l10n::~l10n()
	{
		ures_close(_resource_table);
	}

	std::pair<std::string, bool> l10n::operator[](const std::string& key) const
	{
		UErrorCode status(U_ZERO_ERROR);
		UResourceBundle* resource_string(ures_getByKey(_resource_table, key.c_str(), NULL, &status));

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("l10n::operator[](const std::string& key) > ", u_errorName(status));
			return std::make_pair(key, false);
		}

		const UChar* string(ures_getString(resource_string, NULL, &status));

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("l10n::operator[](const std::string& key) > ", u_errorName(status));
			return std::make_pair(key, false);
		}

		// Perform pre-flighting
		int32_t string_length(0);
		u_strToUTF8(NULL, 0, &string_length, string, -1, &status);

		// Reset pre-flighting errors
		status = U_ZERO_ERROR;

		char result[string_length + 1];
		u_strToUTF8(result, string_length + 1, NULL, string, -1, &status);

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("l10n::operator[](const std::string& key) > ", u_errorName(status));
			return std::make_pair(key, false);
		}

		ures_close(resource_string);
		return std::make_pair(std::string(result), true);
	}

	const std::string& l10n::get_extension_name() const
	{
		return _extension_name;
	}

	const std::string& l10n::get_locale() const
	{
		return _locale;
	}

	const std::string& l10n::get_resource_table_key() const
	{
		return _resource_table_key;
	}
} }

namespace std {
	size_t hash<shared_ptr<chaos::globus::l10n>>::operator()(const shared_ptr<chaos::globus::l10n>& localization) const
	{
		return !localization ? 0 : hash<std::string>()(localization->get_extension_name() + localization->get_locale() + localization->get_resource_table_key());
	}
}
