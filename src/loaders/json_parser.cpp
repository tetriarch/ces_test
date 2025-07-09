#include "json_parser.hpp"

auto JSONParser::valueTypeToTypeID(const json& value) const -> std::type_index {
    if(value.is_null()) return typeid(nullptr);
    if(value.is_string()) return typeid(std::string);
    if(value.is_boolean()) return typeid(bool);
    if(value.is_number_unsigned()) return typeid(u32);
    if(value.is_number_integer()) return typeid(s32);
    if(value.is_number_float()) return typeid(f32);

    return typeid(void);
}
