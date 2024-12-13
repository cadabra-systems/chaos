/**
 @file PIDFile.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_PIDFile_hpp
#define Chaos_PIDFile_hpp

#include <string>
#include <signal.h>

namespace chaos {
	class pid_file
	{
	/** @name Statics */
	/** @{ */
	public:
		static pid_t exists(const std::string& filename);
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		pid_file();
		pid_file(pid_file&& origin) = delete;
		pid_file(const pid_file& origin) = delete;
		~pid_file();
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:

	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::string _filename;
		int _handle;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	private:

	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:

	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		bool close();
		bool lock(const std::string& filename);
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
