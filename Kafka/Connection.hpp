/**
 @file Connection.hpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Kafka_Connection_hpp
#define Chaos_Kafka_Connection_hpp

#include <librdkafka/rdkafka.h>

#include <map>
#include <set>
#include <string>
#include <utility>

namespace chaos { namespace kafka {
	class connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		connection(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map);
		connection(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map);
		connection(connection&&);
		connection(const connection&) = delete;
		virtual ~connection();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::set<std::pair<std::string, std::uint16_t>> _broker_set;
		std::map<std::string, std::string> _configuration_map;

	protected:
		rd_kafka_t* _handle;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		connection& operator=(connection&&) = delete;
		connection& operator=(const connection&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool configure(const std::map<std::string, std::string>& map);
		bool configure(const std::string& key, const std::string& value);
		bool configure(std::nullptr_t);

		virtual bool connect(const std::string& username = "", const std::string& password = "") noexcept;
		virtual bool disconnect() noexcept;
		bool reconnect() noexcept;

		virtual bool alive(bool revive) noexcept;

		bool create_topic(const std::string& name, int partition_count = -1, int replication_factor = -1, int timeout = -1) noexcept;
		bool delete_topic(const std::string& name, int timeout = -1) noexcept;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::map<std::string, std::string>& get_configuration_map() const noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_active() const noexcept;
	/** @} */
	};
} }

#endif
