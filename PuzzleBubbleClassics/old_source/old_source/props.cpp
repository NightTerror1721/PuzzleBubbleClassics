#include "props.h"
#include "resources.h"

namespace gprops
{
	JsonObject __props;

	Path get_filepath() { return resource::data / "props.json"; }

	void clear_properties() { __props.clear(); }

	void load()
	{
		clear_properties();
		__props = utils::json::read(get_filepath());
	}

	void store()
	{
		utils::json::write(get_filepath(), __props);
	}

	const std::optional<Json> get_property(const String& name)
	{
		auto it = __props.find(name);
		if (it == __props.end())
			return {};
		return it->second;
	}

	void set_property(const String& name, const Json& value)
	{
		__props.emplace(name, value);
	}

	bool delete_property(const String& name)
	{
		auto it = __props.find(name);
		if (it != __props.end())
		{
			__props.erase(it);
			return true;
		}
		return false;
	}

	void dump(std::ostream& os, unsigned int indent_step)
	{
		utils::json::write(os, __props, indent_step);
	}
}
