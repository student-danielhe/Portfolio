#include"Engine/Core/NamedStrings.hpp"

void			NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element) {
	XmlAttribute const* attri=element.FirstAttribute();
	while (attri)
	{
		m_keyValuePairs.insert({ attri->Name(), "N/A" });
		attri = attri->Next();
	}
	for (auto mapPair = m_keyValuePairs.begin(); mapPair != m_keyValuePairs.end();++mapPair) {
		std::string notFound = "Error: Attribute Not Found";
		mapPair->second = ParseXmlAttribute(element, mapPair->first.c_str(), notFound);
	}
}
void			NamedStrings::SetValue(std::string const& keyName, std::string const& newValue) {
	m_keyValuePairs[keyName] = newValue;
}
std::string		NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		return value->second;
	}
	return defaultValue;
}
bool			NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		return value->second=="true"|| value->second == "TRUE";
	}
	return defaultValue;
}
int			    NamedStrings::GetValue(std::string const& keyName, int defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		return atoi(value->second.c_str());
	}
	return defaultValue;
}
float			NamedStrings::GetValue(std::string const& keyName, float defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		return static_cast<float>(atof(value->second.c_str()));
	}
	return defaultValue;
}
std::string		NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		return value->second;
	}
	return defaultValue;
}
Rgba8			NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		Rgba8 result;
		result.SetFromText(value->second.c_str());
		return result;
	}
	return defaultValue;
}
Vec2			NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		Vec2 result;
		result.SetFromText(value->second.c_str());
		return result;
	}
	return defaultValue;
}
IntVec2		    NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const {
	auto value = m_keyValuePairs.find(keyName);
	if (value != m_keyValuePairs.end()) {
		IntVec2 result;
		result.SetFromText(value->second.c_str());
		return result;
	}
	return defaultValue;
}

std::string NamedStrings::GetAsString() const
{
    std::string result;
	for (auto pairing : m_keyValuePairs) {
		if (pairing.first.compare("CMD") == 0) {
			result+=pairing.second+" ";
		}else
	        result+=pairing.first +"=" +pairing.second+" ";
	}
	return result;
}
