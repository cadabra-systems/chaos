/**
 @file FiscusClient.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_CWS_FiscusClient_hpp
#define Chaos_CWS_FiscusClient_hpp

#include "../WebAPI.hpp"

#include "../UID.hpp"
#include "../EmailAddress.hpp"
#include "../PhoneNumber.hpp"
#include "../Money.hpp"
#include "../TriBool.hpp"
#include "../Flex.hpp"
#include "../Flex/UID.hpp"

#include <list>
#include <string>

namespace chaos { namespace cws {
	class fiscus_client : protected web_api
	{
	/** @name Classes */
	/** @{ */
	public:
		struct cheque
		{
			struct item
			{
				std::string sku;
				std::string name;
				double quantity;
				money::centum_type price;
			};

			uid guid;
			chaos::uri endpoint;
			std::chrono::time_point<std::chrono::system_clock> issue;
			std::chrono::time_point<std::chrono::system_clock> refund;
			std::chrono::time_point<std::chrono::system_clock> punch;
			email_address customer_email_address;
			phone_number customer_phone_number;
			money::currency currency;
			std::string web_engine; /// < woocommerce, tilda, realmec, taplink, aerarium, telebot
			std::string payment_system;
			std::list<item> item_list;

			std::set<std::string> make_sku_set() const;
			bool is_punched() const;
			bool has_refund() const;
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		fiscus_client(const uri& uri);
		fiscus_client(const std::string& hostname, const std::string& shop_key, bool secure);
		fiscus_client(fiscus_client&& origin);
		fiscus_client(fiscus_client const&) = delete;
		virtual ~fiscus_client();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _shop_key;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	private:
		cheque sign(cheque&& origin) const;
		std::list<cheque> sign(std::list<cheque>&& origin) const;
		bool search(bool verbose, const std::set<std::string>& sku_list, const std::set<email_address>& email_address_list, const std::set<phone_number>& phone_number_list, const tribool& punch, const tribool& refund, const std::pair<std::chrono::time_point<std::chrono::system_clock>, std::chrono::time_point<std::chrono::system_clock>>& timeframe);

	public:
		uid record(const std::string& engine, const flex& payload);
		std::list<uid> seek(const std::set<std::string>& sku_list, const std::set<email_address>& email_address_list, const std::set<phone_number>& phone_number_list, const tribool& punch, const tribool& refund, const std::pair<std::chrono::time_point<std::chrono::system_clock>, std::chrono::time_point<std::chrono::system_clock>>& timeframe);
		std::list<cheque> scan(const std::set<std::string>& sku_list, const std::set<email_address>& email_address_list, const std::set<phone_number>& phone_number_list, const tribool& punch, const tribool& refund, const std::pair<std::chrono::time_point<std::chrono::system_clock>, std::chrono::time_point<std::chrono::system_clock>>& timeframe);
		cheque check(const uid& guid);
		bool punch(const uid& guid);
		bool refund(const uid& guid);
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		fiscus_client& operator=(fiscus_client&& right) = delete;
		fiscus_client& operator=(fiscus_client&) = delete;
		fiscus_client& operator=(fiscus_client const&) = delete;
		fiscus_client& operator=(fiscus_client const&&) = delete;
	/** @} */
	};

	void to_flex(flex& output, const fiscus_client::cheque::item& input);
	void from_flex(const flex& input, fiscus_client::cheque::item& output);

	void to_flex(flex& output, const fiscus_client::cheque& input);
	void from_flex(const flex& input, fiscus_client::cheque& output);
} }

#endif
