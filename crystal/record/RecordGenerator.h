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

#pragma once

#include <string>
#include <vector>

#include "crystal/record/RecordConfig.h"

namespace crystal {

class StringGenerator {
 public:
  StringGenerator(std::string* out) : out_(out) {}

  void append(const std::string& str);
  void append(const std::string& tpl,
              const std::map<std::string, std::string>& map);

 protected:
  std::string* out_;
};

class RecordClassGenerator : public StringGenerator {
 public:
  RecordClassGenerator(std::string* out,
                       const std::string& classname,
                       const std::vector<FieldConfig>* configs)
      : StringGenerator(out), classname_(classname), configs_(configs) {}

  void generate();
  void generateMemberMethod(const FieldConfig& config, size_t i);
  void generateToTuple();

 private:
  std::string toTypeString(const FieldConfig& config);

  std::string classname_;
  const std::vector<FieldConfig>* configs_;
};

class RecordFileGenerator : public StringGenerator {
 public:
  RecordFileGenerator(std::string* out,
                      const std::string& classname,
                      const RecordConfig* config)
      : StringGenerator(out), classname_(classname), config_(config) {}

  void generate();
  void generateHeader();
  void generateFooter();
  void generateRecord();

 private:
  std::string classname_;
  const RecordConfig* config_;
};

} // namespace crystal
