#pragma once
#include <string>
#include <vector>
#include <numeric>
#include "json.hpp"

namespace cxxmodel {
  using namespace nlohmann;

  enum class Access : uint8_t {
    UNKNOWN = 0,
    PUBLIC = 1,
    PRIVATE = 2,
    PROTECTED = 3,
  };

  template <typename ConstFwdIter>
  std::string join(ConstFwdIter beg, ConstFwdIter end, std::string const& sep = " ");
  std::vector<std::string> split(std::string const& s, std::string const& delimiter);

  struct SourceLocation {
    std::string file;
    uint32_t line{0};
  };


  struct Namespace : std::vector<std::string> {
    static inline std::string const sep{"::"};
  };

  struct Class {
  public:
    /* ========== Type ========== */
    struct Field {
      Access access{Access::UNKNOWN};
      std::string name;
      std::string type;
    };

    struct Method {
      struct Parameter {
        std::string name;
        std::string type;
        std::string default_value;
      };

      bool is_pure_virtual{false};
      bool is_virtual{false};
      bool is_const{false};
      bool is_defaulted{false};
      bool is_static{false};
      bool is_implicit{false};
      bool is_delete{false};
      bool is_deprecated{false};
      Access access{Access::UNKNOWN};
      std::string name;
      std::string type;
      std::vector<Class::Method::Parameter> parameters;
    };

    struct Base {
      std::string name;
      bool is_virtual{false};
      Access access{Access::UNKNOWN};
    };

    using ID = int64_t;

    /* ========== Function ========== */
    [[nodiscard]] std::string fullname() const {
      return join(ns.cbegin(), ns.cend(), ns.sep) + ns.sep + name;
    }

    /* ========== Members ========== */
    std::vector<Class::Field> fields;
    std::vector<Class::Method> methods;
    std::vector<Class::Base> bases;
    Namespace ns;
    std::string name;
    SourceLocation source_location;
    bool is_struct{false};
    ID id;
  };

  void to_json(json& j, Access const& access);
  void from_json(json const& j, Access& access);

  void to_json(json& j, SourceLocation const& sourceLocation);
  void from_json(json const& j, SourceLocation& sourceLocation);

  void to_json(json& j, Namespace const& ns);
  void from_json(json const& j, Namespace& ns);

  void to_json(json& j, Class::Field const& field);
  void from_json(json const& j, Class::Field& field);

  void to_json(json& j, Class::Method::Parameter const& parameter);
  void from_json(json const& j, Class::Method::Parameter& parameter);

  void to_json(json& j, Class::Method const& method);
  void from_json(json const& j, Class::Method& method);

  void to_json(json& j, Class::Base const& base);
  void from_json(json const& j, Class::Base& base);

  void to_json(json& j, Class const& cls);
  void from_json(json const& j, Class& cls);


  std::string toPlantUML(Class const& cls);
}  // namespace cxxmodel

