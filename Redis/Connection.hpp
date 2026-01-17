/**
 * @file Connection.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Connection_hpp
#define Chaos_Redis_Connection_hpp

#include "Redis.hpp"
#include "Command.hpp"

#include <list>
#include <memory>
#include <string>

namespace chaos { namespace redis {
	class connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		connection(const std::string& host, std::uint16_t port = 6379, const std::string& name = "");
		connection(connection const&) = delete;
		connection(connection&& origin);
		virtual ~connection();
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		const std::string _host;
		const std::uint16_t _port;
		const std::string _name;

		redisContext* _context;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		connection& operator=(connection&&) = delete;
		connection& operator=(connection const&) = delete;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual bool connect(const std::string& username = "", const std::string& password = "");
		virtual bool disconnect();
		virtual bool reconnect();

		virtual bool alive() = 0;

		virtual bool send(const std::shared_ptr<procedure>& procedure) = 0;
		template<typename P, typename ...PArgs>
		inline typename std::enable_if<std::is_base_of<procedure, P>::value, command<typename P::return_type>>::type
		call(PArgs... args)
		{
			std::shared_ptr<P> retval(new P(std::forward<PArgs>(args)...));
			send(retval);
			return {std::move(retval)};
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_active() const;
		bool is_faulty() const;
	/** @} */
	};

	class sync_connection : public connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		sync_connection(const std::string& host, std::uint16_t port = 6379, const std::string& name = "");
		sync_connection(sync_connection const&) = delete;
		sync_connection(sync_connection&& origin);
		virtual ~sync_connection() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		sync_connection& operator=(sync_connection&&) = delete;
		sync_connection& operator=(sync_connection const&) = delete;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual bool alive() override;

		virtual bool send(const std::shared_ptr<procedure>& procedure) override;
	/** @} */
	};

	class async_connection : public connection
	{
	public:
		async_connection(const std::string& host, const std::uint16_t port = 6379, const std::string& name = "");
		async_connection(const async_connection& origin) = delete;
		async_connection(sync_connection&& origin);
		async_connection(async_connection&& origin);
		virtual ~async_connection() override;

	private:
		std::list<std::shared_ptr<procedure>> _list;

	public:
		async_connection* operator=(const async_connection& rhs) = delete;
		async_connection* operator=(async_connection&& rhs) = delete;

	public:
		virtual bool reconnect() override;

		virtual bool alive() override;

		inline bool send(const std::shared_ptr<procedure>& procedure) override;
		bool send(bool onoff = true);
		bool unsend();
	};
} }

#endif
