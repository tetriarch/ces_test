#pragma once

#include "log.hpp"
#include "utils.hpp"

#include <nlohmann/json.hpp>
using namespace nlohmann;

enum class JSONParserError {
    PARSE
};

class JSONParser {

public:
    virtual ~JSONParser() = default;

protected:
    template<typename T>
    bool get(const json& object, std::string key, bool required, T& result, const std::string& parent = "");

    template<typename T>
    auto typeToString() -> std::string;

    auto valueTypeToTypeID(const json& value) -> std::type_index;
    virtual auto error(const std::string& msg, const std::string& parent = "") -> std::string = 0;
};

template<typename T>
inline bool JSONParser::get(const json& object, std::string key, bool required, T& result, const std::string& parent) {

    json::const_iterator it = object.find(key);
    if(it == object.end()) {
        if(required) {
            std::string err = key + " not found";
            if(!parent.empty()) {
                err += " in " + parent;
            }
            ERROR(err);
        }
        return false;
    }

    if(valueTypeToTypeID(it.value()) != typeid(T)) {
        if(required) {
            std::string err = key + " is a " + it.value().type_name() + ", expected " + typeToString<T>();
            if(!parent.empty()) {
                err = err + " in " + parent;
            }
            ERROR(err);
        }
        return false;
    }
    result = it->get<T>();
    return true;
}

template<typename T>
inline auto JSONParser::typeToString()->std::string {

    if constexpr(std::is_same_v<T, std::string>) return "string";
    if constexpr(std::is_same_v<T, s32>) return "int";
    if constexpr(std::is_same_v < T, u32>) return "unsigned int";
    if constexpr(std::is_same_v<T, f32>) return "float";
    if constexpr(std::is_same_v<T, bool>) return "bool";

    else {
        return "unknown";
    }
}