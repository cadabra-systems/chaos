/**
 @file Document.hpp
 @date 26.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Node_Document_hpp
#define Chaos_CXML_Node_Document_hpp

#include "../AbstractNode.hpp"

namespace chaos { namespace cxml {
	class document : public abstract_node
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		document(const std::string name = "document");
		virtual ~document() = default;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _name;

	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual std::ostream& feed(std::ostream& stream) const override;
	/** @} */
		
	/** @name Converters */
	/** @{ */
	public:
	/** @} */
	
	/** @name Setters */
	/** @{ */
	private:
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		virtual const std::shared_ptr<abstract_node> get_parent() const override;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		virtual bool is_orphan() const override;
	/** @} */
		
	/** @name Hooks */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
