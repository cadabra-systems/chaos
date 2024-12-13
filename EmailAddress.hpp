#/**
 @file EmailAddress.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_EmailAddress_hpp
#define Chaos_EmailAddress_hpp

#include "String.hpp"
#include "Base64.hpp"

#include <string>
#include <regex>

namespace chaos {
   class email_address
   {
   /** @name Constructors */
   /** @{ */
   public:
	   static bool validate(const std::string& email)
	   {
		   if (email.empty()) {
			   return false;
		   }

		   std::smatch email_match;
		   std::regex_search(email, email_match, std::regex("(?:(?:[^<>()\\[\\].,;:\\s@\"]+(?:\\.[^<>()\\[\\].,;:\\s@\"]+)*)|\".+\")@(?:(?:[^<>()‌​\\[\\].,;:\\s@\"]+\\.)+[^<>()\\[\\].,;:\\s@\"]{2,})"));
		   return !email_match.empty();
	   }
   /** @} */

   /** @name Constructors */
   /** @{ */
   public:
	   email_address(const std::string& email = "", const std::string& name = "")
	   :
		   _email(string::lowercased_string(email)),
		   _name(name)
	   {

	   }

	   email_address(const email_address& origin)
	   :
		   _email(origin._email),
		   _name(origin._name)
	   {

	   }

	   email_address(email_address&& origin)
	   :
		   _email(origin._email),
		   _name(origin._name)
	   {

	   }

	   ~email_address() = default;
   /** @} */

   /** @name Properties */
   /** @{ */
   private:
	   std::string _email;
	   std::string _name;
   /** @} */

   /** @name Operators */
   /** @{ */
   public:
	   email_address& operator=(const email_address& rhs)
	   {
		   _email = rhs._email;
		   _name = rhs._name;

		   return *this;
	   }

	   email_address& operator=(email_address&& rhs)
	   {
		   _email = std::move(rhs._email);
		   _name = std::move(rhs._name);

		   return *this;
	   }

	   bool operator<(const email_address& rhs) const
	   {
		   return _email < rhs._email;
	   }
   /** @} */

   /** @name Procedures */
   /** @{ */
   public:
   /** @} */

   /** @name Setters */
   /** @{ */
   public:
	   void email(const std::string& email, bool only_valid = false)
	   {
		   if (only_valid && !email_address::validate(email)) {
			   return;
		   }

		   _email = string::lowercased_string(email);
	   }

	   void name(const std::string& name)
	   {
		   _name = name;
	   }
   /** @} */

   /** @name Getters */
   /** @{ */
   public:
	   const std::string& email() const
	   {
		   return  _email;
	   }

	   const std::string& address() const
	   {
		   return  _email;
	   }

	   const std::string hostname() const
	   {
			const std::string::size_type at(_email.find('@'));
			return (std::string::npos != at) ? _email.substr(at) : "";
	   }

	   const std::string& name() const
	   {
		   return _name;
	   }

	   /**
		* @brief From: Nadezda Megrabyan <spnadka@gmail.com>
		* =?utf-8?b?".base64_encode($s)."?=
		* =?UTF-8?B?0KXRg9C00LXQtdC8LtCe0L3Qu9Cw0LnQvQ==?= <ace@cadabra.systems>
		* @return
		*/
	   std::string rfc() const
	   {
		   return _name.empty() ? ("<" + _email + ">") : ("=?UTF-8?B?" + base64::encode(_name) + "?= <" + _email + ">");
	   }
   /** @} */

   /** @name States */
   /** @{ */
   public:
	   bool valid() const
	   {
		   return email_address::validate(_email);
	   }
   /** @} */
   };
}

#endif
