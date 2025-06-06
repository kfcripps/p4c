/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "json.h"

#include <sstream>
#include <stdexcept>

#include "absl/strings/str_cat.h"
#include "indent.h"
#include "lib/big_int_util.h"

namespace P4::Util {

cstring IJson::toString() const {
    std::stringstream str;
    serialize(str);
    return cstring(str.str());
}

void IJson::dump() const { std::cout << toString(); }

JsonValue *JsonValue::null = new JsonValue();

JsonValue::JsonValue(long long v) : tag(Kind::Integer), intValue(v) {}

JsonValue::JsonValue(unsigned long long v) : tag(Kind::Integer), intValue(v) {}

void JsonValue::serialize(std::ostream &out) const {
    switch (tag) {
        case Kind::String:
            out << "\"" << str << "\"";
            break;
        case Kind::Integer:
            out << intValue;
            break;
        case Kind::Float:
            out << floatValue;
            break;
        case Kind::True:
            out << "true";
            break;
        case Kind::False:
            out << "false";
            break;
        case Kind::Null:
            out << "null";
            break;
    }
}

bool JsonValue::operator==(const big_int &v) const {
    return tag == Kind::Integer ? v == intValue : false;
}
bool JsonValue::operator==(const double &v) const {
    return tag == Kind::Float     ? floatValue == v
           : tag == Kind::Integer ? static_cast<double>(intValue) == v
                                  : false;
}
bool JsonValue::operator==(const float &v) const { return *this == static_cast<double>(v); }
bool JsonValue::operator==(const cstring &s) const {
    return tag == Kind::String ? s == str : false;
}
bool JsonValue::operator==(const std::string &s) const {
    // Note that it does not make sense to convert `s` to `cstring` here. Such
    // conversion involves cstring cache lookup and strcmp() in any case. Here
    // we just doing strcmp() saving 1-2 map lookups
    return tag == Kind::String ? str == s : false;
}
bool JsonValue::operator==(const char *s) const {
    // Note that it does not make sense to convert `s` to `cstring` here. Such
    // conversion involves cstring cache lookup and strcmp() in any case. Here
    // we just doing strcmp() saving 1-2 map lookups
    return tag == Kind::String ? str == s : false;
}

bool JsonValue::operator==(const JsonValue &other) const {
    if (tag != other.tag) return false;
    switch (tag) {
        case Kind::String:
            return str == other.str;
        case Kind::Integer:
            return intValue == other.intValue;
        case Kind::Float:
            return floatValue == other.floatValue;
        case Kind::True:
        case Kind::False:
        case Kind::Null:
            return true;
        default:
            throw std::logic_error("Unexpected json tag");
    }
}

void JsonArray::serialize(std::ostream &out) const {
    bool isSmall = true;
    for (auto v : *this) {
        if (!v->is<JsonValue>()) isSmall = false;
    }
    out << "[";
    if (!isSmall) out << IndentCtl::indent;
    bool first = true;
    for (auto v : *this) {
        if (!first) {
            out << ",";
            if (isSmall) out << " ";
        }
        if (!isSmall) out << IndentCtl::endl;
        if (v == nullptr)
            out << "null";
        else
            v->serialize(out);
        first = false;
    }
    if (!isSmall) out << IndentCtl::unindent << IndentCtl::endl;
    out << "]";
}

bool JsonValue::getBool() const {
    if (!isBool()) throw std::logic_error("Incorrect json value kind");
    return tag == Kind::True;
}

cstring JsonValue::getString() const {
    if (!isString()) throw std::logic_error("Incorrect json value kind");
    return str;
}

big_int JsonValue::getIntValue() const {
    if (!isInteger()) throw std::logic_error("Not an integer");
    return intValue;
}

double JsonValue::getFloatValue() const {
    if (!isFloat()) throw std::logic_error("Not a float");
    return floatValue;
}

int JsonValue::getInt() const {
    auto val = getIntValue();
    if (val < INT_MIN || val > INT_MAX) throw std::logic_error("Value too large for int");
    return static_cast<int>(val);
}

JsonArray *JsonArray::append(IJson *value) {
    push_back(value);
    return this;
}

void JsonObject::serialize(std::ostream &out) const {
    out << "{" << IndentCtl::indent;
    bool first = true;
    for (auto &it : *this) {
        if (!first) out << ",";
        first = false;
        out << IndentCtl::endl;
        out << "\"" << it.first << "\"" << " : ";
        if (it.second == nullptr)
            out << "null";
        else
            it.second->serialize(out);
    }
    out << IndentCtl::unindent << IndentCtl::endl << "}";
}

JsonObject *JsonObject::emplace(cstring label, IJson *value) {
    if (label.isNullOrEmpty()) throw std::logic_error("Empty label");
    auto j = get(label);
    if (j != nullptr) {
        cstring s = value->toString();
        throw std::logic_error(std::string("Attempt to add to json object a value "
                                           "for a label which already exists ") +
                               label.string() + " " + s.string());
    }
    base::emplace(label, value);
    return this;
}

JsonObject *JsonObject::emplace(std::string_view label, IJson *value) {
    if (label.empty()) throw std::logic_error("Empty label");
    auto j = get(label);
    if (j != nullptr) {
        cstring s = value->toString();
        throw std::logic_error(
            absl::StrCat("Attempt to add to json object a value "
                         "for a label which already exists ",
                         label, " ", s));
    }
    base::emplace(label, value);
    return this;
}

JsonObject *JsonObject::emplace_non_null(cstring label, IJson *value) {
    if (value != nullptr) {
        return emplace(label, value);
    }
    return this;
}

}  // namespace P4::Util
