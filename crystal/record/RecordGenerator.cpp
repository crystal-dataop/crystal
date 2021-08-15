/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "crystal/record/RecordGenerator.h"

namespace crystal {

static void replace(std::string& s, std::string k, const std::string& v) {
  k = "{{ " + k + " }}";
  size_t it;
  while ((it = s.find(k)) != s.npos) {
    s.replace(it, k.size(), v);
  }
}

static std::string toCamelCase(const std::string& s) {
  std::string out;
  std::vector<std::string> words;
  split('_', s, words, true);
  for (auto& word : words) {
    word[0] = toupper(word[0]);
    out += word;
  }
  return out;
}

void StringGenerator::append(const std::string& str) {
  *out_ += str;
}

void StringGenerator::append(
    const std::string& tpl, const std::map<std::string, std::string>& map) {
  std::string str = tpl;
  for (auto& kv : map) {
    replace(str, kv.first, kv.second);
  }
  append(str);
}

void RecordClassGenerator::generate() {
  for (auto& config : *configs_) {
    if (config.type() == DataType::TUPLE) {
      RecordClassGenerator gen(
          out_,
          toCamelCase(config.name()) + "Record",
          &config.tuple());
      gen.generate();
    }
  }
  append(R"(
class {{ classname }} : public RecordBase {
 public:
  {{ classname }}() = default;

  {{ classname }}(const untyped_tuple::meta& meta)
      : RecordBase(meta) {}

  {{ classname }}(const untyped_tuple::meta& meta, void* buffer)
      : RecordBase(meta, buffer) {}
)", {{"classname", classname_}});
  for (size_t i = 0; i < configs_->size(); ++i) {
    generateMemberMethod((*configs_)[i], i);
  }
  generateToTuple();
  append(R"(};
)");
}

void RecordClassGenerator::generateMemberMethod(
    const FieldConfig& config, size_t i) {
  if (config.type() == DataType::TUPLE) {
    append(R"(
  {{ basetype }} make{{ basetype }}() {
    return {{ basetype }}(
        untyped_tuple::meta{untyped_tuple_.meta_[{{ i }}].submeta});
  }
)", {{"basetype", toCamelCase(config.name()) + "Record"},
     {"i", to<std::string>(i)}});
  }
  append(R"(
  {{ type }}& {{ name }}() {
    return get<{{ type }}>({{ i }});
  }
  const {{ type }}& {{ name }}() const {
    return get<{{ type }}>({{ i }});
  }
)", {{"type", toTypeString(config)},
     {"name", config.name()},
     {"i", to<std::string>(i)}});
}

void RecordClassGenerator::generateToTuple() {
  std::vector<std::string> types;
  for (auto& config : *configs_) {
    types.push_back(toTypeString(config));
  }
  append(R"(
  using tuple_type = tuple<{{ types }}>;

  tuple_type& toTuple() {
    return *reinterpret_cast<tuple_type*>(this);
  }
  const tuple_type& toTuple() const {
    return *reinterpret_cast<const tuple_type*>(this);
  }
)", {{"types", join(", ", types)}});
}

std::string RecordClassGenerator::toTypeString(const FieldConfig& config) {
  switch (config.type()) {
#define CASE(dt, t)                                                 \
    case DataType::dt:                                              \
      if (config.count() == 0)                                      \
        return "vector<" #t ">";                                    \
      else if (config.count() == 1)                                 \
        return #t;                                                  \
      else                                                          \
        return stringPrintf("array<" #t ", %zu>", config.count());

    CASE(BOOL, bool)
    CASE(INT8, int8_t)
    CASE(INT16, int16_t)
    CASE(INT32, int32_t)
    CASE(INT64, int64_t)
    CASE(UINT8, uint8_t)
    CASE(UINT16, uint16_t)
    CASE(UINT32, uint32_t)
    CASE(UINT64, uint64_t)
    CASE(FLOAT, float)
    CASE(DOUBLE, double)
    CASE(STRING, string)

    case DataType::TUPLE: {
      std::string type = toCamelCase(config.name()) + "Record";
      if (config.count() == 0)
        return "vector<" + type + ">";
      else if (config.count() == 1)
        return type;
      else
        return stringPrintf("array<%s, %zu>", type.c_str(), config.count());
    }
    default: break;

#undef CASE
  }
  return "";
}

void RecordFileGenerator::generate() {
  generateHeader();
  generateRecord();
  generateFooter();
}

void RecordFileGenerator::generateHeader() {
  append(R"(/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Generate by crystal-recordgen
 * !!! DO NOT EDIT THIS FILE !!!
 */

#pragma once

#include "crystal/record/RecordBase.h"

namespace crystal {
)");
}

void RecordFileGenerator::generateFooter() {
  append(R"(
} // namespace crystal)");
}

void RecordFileGenerator::generateRecord() {
  std::string camel = toCamelCase(filename_);
  RecordClassGenerator gen(
      out_,
      endsWith(camel, "Record") ? camel : camel + "Record",
      &configs_);
  gen.generate();
}

} // namespace crystal
