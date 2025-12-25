#include"Engine/Core/XmlUtils.hpp"


int         ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute!=nullptr) {
		return atoi(attribute);
	}
	return defaultValue;
}
char        ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return attribute[0];
	}
	return defaultValue;
}
bool        ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return !strcmp(attribute,"true");
	}
	return defaultValue;
}
float       ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return static_cast<float>(atof(attribute));
	}
	return defaultValue;
}
Rgba8       ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		Rgba8 result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}
Vec2        ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		Vec2 result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}
IntVec2     ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		IntVec2 result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return attribute;
	}
	return defaultValue;
}
Strings     ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimeter) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return SplitStringOnDelimiter(attribute,delimeter);
	}
	return defaultValues;
}
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		return attribute;
	}
	return defaultValue;
}

Vec3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		Vec3 result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		EulerAngles result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}
FloatRange ParseXmlAttribute(XmlElement const& element, char const* attributeName, FloatRange const& defaultValue) {
	const char* attribute = element.Attribute(attributeName);
	if (attribute != nullptr) {
		FloatRange result;
		result.SetFromText(attribute);
		return result;
	}
	return defaultValue;
}