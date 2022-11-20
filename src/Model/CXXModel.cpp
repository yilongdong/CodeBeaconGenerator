#include "Beacon/Model/CXXModel.h"

namespace cxxmodel {
  template <typename ConstFwdIter>
  std::string join(ConstFwdIter beg, ConstFwdIter end, std::string const& sep) {
    if (beg == end) return "";
    return std::accumulate(std::next(beg), end, *beg, [&sep](auto const& a, auto const& b) { return a + sep + b; });
  }

  std::vector<std::string> split(std::string const& s, std::string const& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;
    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
  }

  void to_json(json& j, Access const& access) {
    switch (access) {
      case Access::PUBLIC: j = "public"; break;
      case Access::PRIVATE: j = "private"; break;
      case Access::PROTECTED: j = "protected"; break;
      default: j = "unknown"; break;
    }
  }
  void from_json(json const& j, Access& access) {
     std::string accessStr = j.get<std::string>();
     if (accessStr == "public") {
       access = Access::PUBLIC;
     }
     else if (accessStr == "private") {
       access = Access::PRIVATE;
     }
     else if (accessStr == "protected") {
       access = Access::PROTECTED;
     }
     else {
       access = Access::UNKNOWN;
     }
  }

  void to_json(json& j, SourceLocation const& sourceLocation) {
    j = {
        {"file", sourceLocation.file },
        {"line", sourceLocation.line },
    };
  }
  void from_json(json const& j, SourceLocation& sourceLocation) {
      sourceLocation.file = j.at("file").get<std::string>();
      sourceLocation.line = j.at("line").get<uint32_t>();
  }

  void to_json(json& j, Namespace const& ns) {
    j = join(ns.cbegin(), ns.cend(), ns.sep);
  }
  void from_json(json const& j, Namespace& ns) {
    auto tokens = split(j.get<std::string>(), ns.sep);
    ns.clear();
    for(auto const& token : tokens) {
      ns.push_back(token);
    }
  }

  void to_json(json& j, Class::Field const& field) {
    j = {
        { "name", field.name },
        { "type", field.type },
        { "access", field.access },
    };
  }
  void from_json(json const& j, Class::Field& field) {
    field.name = j.at("name").get<std::string>();
    field.type = j.at("type").get<std::string>();
    field.access = j.at("access").get<Access>();
  }

  void to_json(json& j, Class::Method::Parameter const& parameter) {
    j = {
      { "name", parameter.name },
      { "type", parameter.type },
      { "default_value", parameter.default_value },
    };
  }
  void from_json(json const& j, Class::Method::Parameter& parameter) {
    parameter.name = j.at("name").get<std::string>();
    parameter.type = j.at("type").get<std::string>();
    parameter.default_value = j.at("default_value").get<std::string>();
  }

  void to_json(json& j, Class::Method const& method) {
    j = {
        { "name", method.name },
        { "type", method.type },
        { "access", method.access },
        { "params", method.parameters },
        { "qualifier",
            { "is_pure_virtual", method.is_pure_virtual },
            { "is_virtual", method.is_virtual },
            { "is_const", method.is_const },
            { "is_defaulted", method.is_defaulted },
            { "is_static", method.is_static },
            { "is_implicit", method.is_implicit },
        },
    };
  }

  void from_json(json const& j, Class::Method& method) {
    method.name = j.at("name").get<std::string>();
    method.type = j.at("type").get<std::string>();
    method.access = j.at("access").get<Access>();
    method.parameters = j.at("params").get<std::vector<Class::Method::Parameter>>();
    method.is_pure_virtual = j.at("is_pure_virtual").get<bool>();
    method.is_virtual = j.at("is_virtual").get<bool>();
    method.is_const = j.at("is_const").get<bool>();
    method.is_defaulted = j.at("is_defaulted").get<bool>();
    method.is_static = j.at("is_static").get<bool>();
    method.is_implicit = j.at("is_implicit").get<bool>();
  }
  void to_json(json& j, Class::Base const& base) {
    j = {
      { "name", base.name },
      { "is_virtual", base.is_virtual },
      { "access", base.access }
    };
  }
  void from_json(json const& j, Class::Base& base) {
    base.name = j.at("name").get<std::string>();
    base.is_virtual = j.at("is_virtual").get<bool>();
    base.access = j.at("access").get<Access>();
  }

  void to_json(json& j, Class const& cls) {
    j = {
        { "name", cls.name },
        { "namespace", cls.ns },
        { "source_location", cls.source_location },
        { "bases", cls.bases },
        { "methods", cls.methods },
        { "fields", cls.fields },
        { "is_struct", cls.is_struct },
    };
  }
  void from_json(json const& j, Class& cls) {
    cls.name = j.at("name").get<std::string>();
    cls.ns = j.at("namespace").get<Namespace>();
    cls.source_location = j.at("source_location").get<SourceLocation>();
    cls.bases = j.at("bases").get<std::vector<Class::Base>>();
    cls.methods = j.at("methods").get<std::vector<Class::Method>>();
    cls.fields = j.at("fields").get<std::vector<Class::Field>>();
    cls.is_struct = j.at("is_struct").get<bool>();
  }


}