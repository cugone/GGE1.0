#include "Engine/Core/DataUtils.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

namespace DataUtils {

void ValidateXmlElement(const XMLElement& element,
    const std::string& name,
    const std::string& requiredChildElements,
    const std::string& requiredAttributes,
    const std::string& optionalChildElements /*= std::string("")*/,
    const std::string& optionalAttributes /*= std::string("")*/)
{
    if (name.empty()) {
        std::ostringstream err_ss;
        err_ss << "Element validation failed. Element name is required.";
        ERROR_AND_DIE(err_ss.str().c_str());
    }
    auto xmlNameAsCStr = element.Name();
    std::string xml_name = xmlNameAsCStr ? xmlNameAsCStr : "";
    if (xml_name != name) {
        std::ostringstream err_ss;
        err_ss << "Element validation failed. Element name \"" << xml_name << "\" does not match valid name \"" << name << "\"\n";
        ERROR_AND_DIE(err_ss.str().c_str());
    }

    std::vector<std::string> requiredAttributeNames = Split(requiredAttributes);
    std::sort(requiredAttributeNames.begin(), requiredAttributeNames.end());

    std::vector<std::string> requiredChildElementNames = Split(requiredChildElements);
    std::sort(requiredChildElementNames.begin(), requiredChildElementNames.end());

    std::vector<std::string> optionalChildElementNames = Split(optionalChildElements);
    std::sort(optionalChildElementNames.begin(), optionalChildElementNames.end());

    std::vector<std::string> optionalAttributeNames = Split(optionalAttributes);
    std::sort(optionalAttributeNames.begin(), optionalAttributeNames.end());

    std::vector<std::string> actualChildElementNames = GetChildElementNames(element);
    std::sort(actualChildElementNames.begin(), actualChildElementNames.end());

    std::vector<std::string> actualAttributeNames = GetAttributeNames(element);
    std::sort(actualAttributeNames.begin(), actualAttributeNames.end());

    //Difference between actual attribute names and required list is list of actual optional attributes.
    std::vector<std::string> actualOptionalAttributeNames;
    std::set_difference(actualAttributeNames.begin(), actualAttributeNames.end(),
        requiredAttributeNames.begin(), requiredAttributeNames.end(),
        std::back_inserter(actualOptionalAttributeNames)
    );
    std::sort(actualOptionalAttributeNames.begin(), actualOptionalAttributeNames.end());

    //Difference between actual child names and required list is list of actual optional children.
    std::vector<std::string> actualOptionalChildElementNames;
    std::set_difference(actualChildElementNames.begin(), actualChildElementNames.end(),
        requiredChildElementNames.begin(), requiredChildElementNames.end(),
        std::back_inserter(actualOptionalChildElementNames)
    );
    std::sort(actualOptionalChildElementNames.begin(), actualOptionalChildElementNames.end());

    //Find missing attributes
    std::vector<std::string> missingRequiredAttributes;
    std::set_difference(requiredAttributeNames.begin(), requiredAttributeNames.end(),
        actualAttributeNames.begin(), actualAttributeNames.end(),
        std::back_inserter(missingRequiredAttributes));

    if (!missingRequiredAttributes.empty()) {
        std::ostringstream err_ss;
        for (auto& c : missingRequiredAttributes) {
            err_ss << "Attribute validation failed. Missing required attribute \"" << c << "\"\n";
        }
        ERROR_AND_DIE(err_ss.str().c_str());
    }

    //Find missing children
    std::vector<std::string> missingRequiredChildren;
    std::set_difference(requiredChildElementNames.begin(), requiredChildElementNames.end(),
        actualChildElementNames.begin(), actualChildElementNames.end(),
        std::back_inserter(missingRequiredChildren));

    if (!missingRequiredChildren.empty()) {
        std::ostringstream err_ss;
        for (auto& c : missingRequiredChildren) {
            err_ss << "Child Element validation failed. Missing required child \"" << c << "\"\n";
        }
        ERROR_AND_DIE(err_ss.str().c_str());
    }

#ifdef _DEBUG
    //Find extra attributes
    std::vector<std::string> extraOptionalAttributes;
    std::set_difference(actualOptionalAttributeNames.begin(), actualOptionalAttributeNames.end(),
        optionalAttributeNames.begin(), optionalAttributeNames.end(),
        std::back_inserter(extraOptionalAttributes));

    if (!extraOptionalAttributes.empty()) {
        std::ostringstream err_ss;
        for (auto& c : extraOptionalAttributes) {
            err_ss << "Optional Attribute validation failed. Unknown attribute \"" << c << "\" found.\n";
        }
        ERROR_AND_DIE(err_ss.str().c_str());
    }

    //Find extra children
    std::vector<std::string> extraOptionalChildren;
    std::set_difference(actualOptionalChildElementNames.begin(), actualOptionalChildElementNames.end(),
        optionalChildElementNames.begin(), optionalChildElementNames.end(),
        std::back_inserter(extraOptionalChildren));

    if (!extraOptionalChildren.empty()) {
        std::ostringstream err_ss;
        for (auto& c : extraOptionalChildren) {
            err_ss << "Optional Child Element validation failed. Unknown child \"" << c << "\" found.\n";
        }
        ERROR_AND_DIE(err_ss.str().c_str());
    }
#endif
}

unsigned int GetAttributeCount(const XMLElement &element) {
    unsigned int attributeCount = 0;
    for(auto attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        ++attributeCount;
    }
    return attributeCount;
}

std::vector<std::string> GetAttributeNames(const XMLElement& element) {
    std::size_t attributeCount = GetAttributeCount(element);
    std::vector<std::string> attributeNames(attributeCount);
    std::size_t attributeIndex = 0;
    for(auto attribute = element.FirstAttribute(); attributeIndex < attributeCount && attribute != nullptr; attribute = attribute->Next()) {
        attributeNames[attributeIndex] = attribute->Name();
        ++attributeIndex;
    }
    return attributeNames;
}

unsigned int GetChildElementCount(const XMLElement &element, const std::string& elementName /*= std::string("")*/) {
    unsigned int childCount = 0;
    const char* elementAsCstr = elementName.empty() ? nullptr : elementName.c_str();
    for(auto childElement = element.FirstChildElement(elementAsCstr);
        childElement != nullptr;
        childElement = childElement->NextSiblingElement(elementAsCstr)) {
        ++childCount;
    }
    return childCount;
}

std::vector<std::string> GetChildElementNames(const XMLElement& element) {
    std::size_t childCount = GetChildElementCount(element);
    std::vector<std::string> childElementNames(childCount);
    std::size_t childIndex = 0;
    for(auto child = element.FirstChildElement(); childIndex < childCount && child != nullptr; child = child->NextSiblingElement()) {
        childElementNames[childIndex] = child->Name();
        ++childIndex;
    }
    return childElementNames;
}

bool ParseXmlElementText(const XMLElement& element, bool defaultValue) {
    bool retVal = defaultValue;
    element.QueryBoolText(&retVal);
    return retVal;
}
unsigned char ParseXmlElementText(const XMLElement& element, unsigned char defaultValue) {
    unsigned int retVal = defaultValue;
    element.QueryUnsignedText(&retVal);
    return static_cast<unsigned char>(retVal);
}
signed char ParseXmlElementText(const XMLElement& element, signed char defaultValue) {
    int retVal = defaultValue;
    element.QueryIntText(&retVal);
    return static_cast<signed char>(retVal);
}
char ParseXmlElementText(const XMLElement& element, char defaultValue) {
    char retVal = defaultValue;
    auto s = element.GetText();
    std::string str(s ? s : "");
    if(str.empty()) {
        return retVal;
    }
    retVal = str[0];
    return retVal;
}
unsigned short ParseXmlElementText(const XMLElement& element, unsigned short defaultValue) {
    unsigned int retVal = defaultValue;
    element.QueryUnsignedText(&retVal);
    return static_cast<unsigned short>(retVal);
}
short ParseXmlElementText(const XMLElement& element, short defaultValue) {
    int retVal = defaultValue;
    element.QueryIntText(&retVal);
    return static_cast<short>(retVal);
}
unsigned int ParseXmlElementText(const XMLElement& element, unsigned int defaultValue) {
    unsigned int retVal = defaultValue;
    element.QueryUnsignedText(&retVal);
    return retVal;
}
int ParseXmlElementText(const XMLElement& element, int defaultValue) {
    int retVal = defaultValue;
    element.QueryIntText(&retVal);
    return retVal;
}
unsigned long ParseXmlElementText(const XMLElement& element, unsigned long defaultValue) {
    unsigned int retVal = defaultValue;
    element.QueryUnsignedText(&retVal);
    return static_cast<unsigned long>(retVal);
}
long ParseXmlElementText(const XMLElement& element, long defaultValue) {
    long long retVal = defaultValue;
    element.QueryInt64Text(&retVal);
    return static_cast<long>(retVal);
}
unsigned long long ParseXmlElementText(const XMLElement& element, unsigned long long defaultValue) {
    long long retVal = defaultValue;
    element.QueryInt64Text(&retVal);
    return static_cast<unsigned long long>(retVal);
}
long long ParseXmlElementText(const XMLElement& element, long long defaultValue) {
    long long retVal = defaultValue;
    element.QueryInt64Text(&retVal);
    return retVal;
}
float ParseXmlElementText(const XMLElement& element, float defaultValue) {
    float retVal = defaultValue;
    element.QueryFloatText(&retVal);
    return retVal;
}
double ParseXmlElementText(const XMLElement& element, double defaultValue) {
    double retVal = defaultValue;
    element.QueryDoubleText(&retVal);
    return retVal;
}
Rgba ParseXmlElementText(const XMLElement& element, const Rgba& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Rgba(textVal);
    }
}
Vector2 ParseXmlElementText(const XMLElement& element, const Vector2& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector2(textVal);
    }
}
IntVector2 ParseXmlElementText(const XMLElement& element, const IntVector2& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector2(textVal);
    }
}
Vector3 ParseXmlElementText(const XMLElement& element, const Vector3& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector3(textVal);
    }
}
IntVector3 ParseXmlElementText(const XMLElement& element, const IntVector3& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector3(textVal);
    }
}
Vector4 ParseXmlElementText(const XMLElement& element, const Vector4& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector4(textVal);
    }
}
IntVector4 ParseXmlElementText(const XMLElement& element, const IntVector4& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector4(textVal);
    }
}
Matrix4 ParseXmlElementText(const XMLElement& element, const Matrix4& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Matrix4(textVal);
    }
}

std::string ParseXmlElementText(const XMLElement& element, const char* defaultValue) {
    std::string textVal_default(defaultValue ? defaultValue : "");
    return ParseXmlElementText(element, textVal_default);
}

std::string ParseXmlElementText(const XMLElement& element, const std::string& defaultValue) {
    auto s = element.GetText();
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return textVal;
    }
}

bool ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, bool defaultValue) {
    bool retVal = defaultValue;
    element.QueryBoolAttribute(attributeName.c_str(), &retVal);
    return retVal;
}

unsigned char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned char defaultValue) {
    unsigned int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<unsigned char>(std::stoul(values[0]));
        }
        unsigned char lower = static_cast<unsigned char>(std::stoul(values[0]));
        unsigned char upper = static_cast<unsigned char>(std::stoul(values[1]));
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return static_cast<unsigned char>(retVal);
}

signed char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, signed char defaultValue) {
    signed int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryIntAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<signed char>(std::stoul(values[0]));
        }
        signed char lower = static_cast<signed char>(std::stoul(values[0]));
        signed char upper = static_cast<signed char>(std::stoul(values[1]));
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return static_cast<signed char>(retVal);
}

char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, char defaultValue) {
    char retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        int retValAsInt = retVal;
        element.QueryIntAttribute(attributeName.c_str(), &retValAsInt);
        retVal = static_cast<char>(retValAsInt);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<char>(std::stoi(values[0]));
        }
        char lower = static_cast<char>(std::stoul(values[0]));
        char upper = static_cast<char>(std::stoul(values[1]));
        retVal = static_cast<char>(MathUtils::GetRandomIntInRange(lower, upper));
    }
    return static_cast<char>(retVal);
}

unsigned short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned short defaultValue) {
    unsigned int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<unsigned short>(std::stoul(values[0]));
        }
        unsigned short lower = static_cast<unsigned short>(std::stoul(values[0]));
        unsigned short upper = static_cast<unsigned short>(std::stoul(values[1]));
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return static_cast<unsigned short>(retVal);
}

short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, short defaultValue) {
    int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryIntAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<short>(std::stoi(values[0]));
        }
        short lower = static_cast<short>(std::stoi(values[0]));
        short upper = static_cast<short>(std::stoi(values[1]));
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return static_cast<unsigned short>(retVal);
}

unsigned int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned int defaultValue) {
    unsigned int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<unsigned short>(std::stoul(values[0]));
        }
        unsigned int lower = static_cast<unsigned int>(std::stoul(values[0]));
        unsigned int upper = static_cast<unsigned int>(std::stoul(values[1]));
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return static_cast<unsigned int>(retVal);
}

int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, int defaultValue) {
    int retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryIntAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stoi(values[0]);
        }
        int lower = std::stoi(values[0]);
        int upper = std::stoi(values[1]);
        retVal = MathUtils::GetRandomIntInRange(lower, upper);
    }
    return retVal;
}

unsigned long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long defaultValue) {
    long long retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryInt64Attribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stoul(values[0]);
        }
        long lower = static_cast<long>(std::stoll(values[0]));
        long upper = static_cast<long>(std::stoll(values[1]));
        retVal = MathUtils::GetRandomLongInRange(lower, upper);
    }
    return static_cast<unsigned long>(retVal);
}

long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long defaultValue) {
    long long retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryInt64Attribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return static_cast<long>(std::stoll(values[0]));
        }
        long lower = static_cast<long>(std::stoll(values[0]));
        long upper = static_cast<long>(std::stoll(values[1]));
        retVal = MathUtils::GetRandomLongInRange(lower, upper);
    }
    return static_cast<long>(retVal);
}

unsigned long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long long defaultValue) {
    long long retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryInt64Attribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stoull(values[0]);
        }
        unsigned long long lower = std::stoull(values[0]);
        unsigned long long upper = std::stoull(values[1]);
        retVal = MathUtils::GetRandomLongLongInRange(lower, upper);
    }
    return static_cast<unsigned long long>(retVal);
}

long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long long defaultValue) {
    long long retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryInt64Attribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stoll(values[0]);
        }
        long long lower = std::stoll(values[0]);
        long long upper = std::stoll(values[1]);
        retVal = MathUtils::GetRandomLongLongInRange(lower, upper);
    }
    return static_cast<long long>(retVal);
}

float ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, float defaultValue) {
    float retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryFloatAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stof(values[0]);
        }
        float lower = std::stof(values[0]);
        float upper = std::stof(values[1]);
        retVal = MathUtils::GetRandomFloatInRange(lower, upper);
    }
    return retVal;
}

double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, double defaultValue) {
    double retVal = defaultValue;
    auto attrAsCStr = element.Attribute(attributeName.c_str());
    auto attr = std::string(attrAsCStr ? attrAsCStr : "");
    bool is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
    } else {
        auto values = Split(attr, '~');
        if(values.size() == 1) {
            return std::stod(values[0]);
        }
        double lower = std::stod(values[0]);
        double upper = std::stod(values[1]);
        retVal = MathUtils::GetRandomDoubleInRange(lower, upper);
    }
    return retVal;
}

Rgba ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Rgba& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Rgba(textVal);
    }
}

Vector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector2& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector2(textVal);
    }
}

IntVector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector2& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector2(Vector2(textVal));
    }
}

Vector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector3& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector3(textVal);
    }
}

IntVector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector3& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector3(Vector3(textVal));
    }
}

Vector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector4& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Vector4(textVal);
    }
}

IntVector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector4& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return IntVector4(Vector4(textVal));
    }

}

Matrix4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Matrix4& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    std::string textVal(s ? s : "");
    if(textVal.empty()) {
        return defaultValue;
    } else {
        return Matrix4(textVal);
    }
}


std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const char* defaultValue) {
    std::string textVal_default(defaultValue ? defaultValue : "");
    return ParseXmlAttribute(element, attributeName, textVal_default);
}


std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const std::string& defaultValue) {
    auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
    return (s ? s : defaultValue);
}


} //end DataUtils