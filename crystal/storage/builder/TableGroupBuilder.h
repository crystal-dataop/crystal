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

#include "crystal/storage/builder/TableBuilder.h"
#include "crystal/storage/table/TableGroup.h"

namespace crystal {

class TableGroupBuilder {
 public:
  TableGroupBuilder(TableGroup* tableGroup);
  virtual ~TableGroupBuilder() {}

  /*
   * ALL:   name.*
   * KV:    name._
   * INDEX: name.index
   */
  bool add(const std::string& path, const dynamic& j);
  bool update(const std::string& path, const dynamic& j);
  bool remove(const std::string& path, const dynamic& j);

 private:
  bool resetRelated(
      dynamic& j, const std::map<std::string, std::string>& related) const;

  TableGroup* tableGroup_;
  std::map<std::string, std::unique_ptr<TableBuilder>> builders_;
};

} // namespace crystal
