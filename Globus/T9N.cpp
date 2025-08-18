/**
 @file T9N.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "T9N.hpp"

#include "../Log.hpp"

#include <cmath>

#include <unicode/calendar.h>
#include <unicode/msgfmt.h>

namespace chaos { namespace globus {
	t9n::t9n(const std::string& resource_path, const std::string& class_name, const std::string& locale)
	:
		_resource_table(l10n::instance(resource_path, class_name, locale))
	{

	}

	std::string t9n::manual_process() const
	{
		std::string result(_input);
		std::size_t left_position(result.find('<'));

		while (left_position != std::string::npos) {
			const std::size_t right_position(result.find('>', left_position + 1));

			if (right_position == std::string::npos) {
				break;
			}

			const int substring_length(right_position - left_position + 1);
			int replacement_string_length(substring_length);

			if (substring_length != 2) {
				const char first_key_character(result.at(left_position + 1));

				if (first_key_character == ':') {
					/// @todo Необходимо реализовать "умную" замену местоимений, на которые указывают половые модификаторы (теги)
					/// Как пример: "Its:<gender> mother..." -> "His mother...", если gender == male
					/// @note В текущей реализации модификаторы просто полностью удаляются

					if (substring_length != 3) {
						replacement_string_length = 0; /// @note Удаление - это фактически замена на строку длины нуль
						result.erase(left_position, substring_length);
					} else {
						/// @note substring == "<:>" (не указано имя аргумента) - игнорируем
					}
				} else if (first_key_character == '+') {
					try {
						const std::size_t ascii_code_start(left_position + 2);
						const int ascii_code(std::stoi(result.substr(ascii_code_start, right_position - ascii_code_start)));

						const std::string ascii_string(std::string(1, static_cast<char>(ascii_code)));
						replacement_string_length = ascii_string.length(); /// @ocd Длина строки (в теории) всегда будет равна единице
						result.replace(left_position, substring_length, ascii_string);
					} catch (...) {
						/// @note substring == "<+>" or "<+abc>" (приведение к int невозможно) - игнорируем
						chaos::log_register<chaos::log>::warning("t9n::manual_process() > Something went wrong while converting std::string to int");
					}
				} else if (first_key_character == '*') {
					/// @todo Необходимо реализовать алгоритм, аналогичный представленному выше для ASCII, но для Unicode-кодов
					/// Как пример: "*0021" -> "!", "*0023" -> "#", "*0025" -> "%"
					/// @note В текущей реализации Unicode-коды игнорируются
				} else {
					const std::size_t argument_name_start(left_position + 1);
					const std::string argument_name(result.substr(argument_name_start, right_position - argument_name_start));

					if (_named_arguments.find(argument_name) != _named_arguments.end()) {
						/// @todo Необходимо реализовать поддержку явного типизирования в шаблонах на Globish
						/// Как пример: "<birthdate/date>" -> приводим значение birthdate к типу date
						/// @note В текущей реализации любой аргумент приводится к типу string

						const icu::UnicodeString icu_string_value((_named_arguments.at(argument_name)).getString());
						std::string std_string_value;

						icu_string_value.toUTF8String(std_string_value);
						replacement_string_length = std_string_value.length();
						result.replace(left_position, substring_length, std_string_value);
					} else {
						/// @note substring == "<non_existent_argument_name>" (именованный аргумент не найден) - игнорируем
					}
				}
			} else {
				/// @note substring == "<>" (пустая пара скобочек) - игнорируем
			}

			left_position = result.find('<', right_position + 1 - std::abs(substring_length - replacement_string_length));
		}

		return result;
	}

	t9n::operator std::string() const
	{
		if (!_resource_table) {
			return manual_process();
		}

		std::pair<std::string, bool> translation((*_resource_table)[_input]);

		if (!translation.second) {
			return manual_process();
		}

		UErrorCode status(U_ZERO_ERROR);
		icu::MessageFormat formatter(icu::UnicodeString::fromUTF8(translation.first), _resource_table->get_locale().c_str(), status);

		if (U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("t9n::operator std::string() > ", u_errorName(status));
			return manual_process();
		}

		std::vector<icu::UnicodeString> argument_names;
		std::vector<icu::Formattable> argument_values;

		for (const std::unordered_map<std::string, icu::Formattable>::value_type& pair : _named_arguments) {
			argument_names.push_back(pair.first.c_str());
			argument_values.push_back(pair.second);
		}

		icu::UnicodeString result_icu_string;
		formatter.format(argument_names.data(), argument_values.data(), _named_arguments.size(), result_icu_string, status);

		if(U_FAILURE(status)) {
			chaos::log_register<chaos::log>::warning("t9n::operator std::string() > ", u_errorName(status));
			return manual_process();
		}

		std::string result_std_string;
		result_icu_string.toUTF8String(result_std_string);
		return result_std_string;
	}

	t9n& t9n::operator()(const std::string& phrase)
	{
		_input = phrase;

		return *this;
	}

	t9n& t9n::operator()(const std::string& key, int number)
	{
		if (!_named_arguments.insert(std::unordered_map<std::string, icu::Formattable>::value_type(key, number)).second) {
			chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, int number) > Saving the named argument failed (most likely the key '", key, "' was respecified)");
		}

		return *this;
	}

	t9n& t9n::operator()(const std::string& key, double number)
	{
		if (!_named_arguments.insert(std::unordered_map<std::string, icu::Formattable>::value_type(key, number)).second) {
			chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, double number) > Saving the named argument failed (most likely the key '", key, "' was respecified)");
		}

		return *this;
	}

	t9n& t9n::operator()(const std::string& key, const std::string& string)
	{
		if (!_named_arguments.insert(std::unordered_map<std::string, icu::Formattable>::value_type(key, string.c_str())).second) {
			chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, const std::string& string) > Saving the named argument failed (most likely the key '", key, "' was respecified)");
		}

		return *this;
	}

	t9n& t9n::operator()(const std::string& key, const chaos::date& date)
	{
		if (!_named_arguments.count(key)) {
			UErrorCode status(U_ZERO_ERROR);
			icu::Calendar* calendar(icu::Calendar::createInstance(_resource_table ? _resource_table->get_locale().c_str() : icu::Locale(), status));

			if (U_FAILURE(status)) {
				chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, const chaos::date& date) > Couldn't create icu::Calendar object");
				return *this;
			}

			calendar->set(date.get_year(), date.get_month() - 1, date.get_day());
			const UDate date_in_milliseconds(calendar->getTime(status));

			if (U_FAILURE(status)) {
				chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, const chaos::date& date) > Invalid date is specified: ", date);
				return *this;
			}

			const icu::Formattable formattable_date(date_in_milliseconds, icu::Formattable::kIsDate);

			if (!_named_arguments.insert(std::unordered_map<std::string, icu::Formattable>::value_type(key, formattable_date)).second) {
				chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, const chaos::date& date) > Saving the named argument failed (most likely the key '", key, "' was respecified)");
				return *this;
			}

			delete calendar;
		} else {
			chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, const chaos::date& date) > The key '", key, "' already exists");
		}

		return *this;
	}

	t9n& t9n::operator()(const std::string& key, gender gender)
	{
		if (!_named_arguments.count(key)) {
			const std::string select_gender((gender == gender::male) ? "male" : (gender == gender::female) ? "female" : "other");

			if (!_named_arguments.insert(std::unordered_map<std::string, icu::Formattable>::value_type(key, select_gender.c_str())).second) {
				chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, t9n::gender gender) > Saving the named argument failed (most likely the key '", key, "' was respecified)");
			}
		} else {
			chaos::log_register<chaos::log>::warning("t9n::operator()(const std::string& key, t9n::gender gender) > The key '", key, "' already exists");
		}

		return *this;
	}
} }
