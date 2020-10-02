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

#include "crystal/operator/generic/Serialize.h"
#include "crystal/operator/search/Search.h"
#include "crystal/operator/test/OperatorTest.h"

using namespace crystal;
using namespace crystal::op;

TEST_F(OperatorTest, Slice) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");
  DataView view(std::make_unique<DocumentArray>(extable));

  std::vector<uint64_t> tokens = {1,2,3,4,5,6,7};
  view | search(tokens);

  EXPECT_STREQ(
      R"([{desc="desc",foodId=1,menuId=4294967297,menu__content="content",menu__food=["a","b","c"],menu__menuId=1,menu__status=2,name="a",price=5.5,status=2})"
      R"(,{desc="desc",foodId=2,menuId=4294967297,menu__content="content",menu__food=["a","b","c"],menu__menuId=1,menu__status=2,name="b",price=4.5,status=2})"
      R"(,{desc="desc",foodId=3,menuId=4294967297,menu__content="content",menu__food=["a","b","c"],menu__menuId=1,menu__status=2,name="c",price=4.5,status=2})"
      R"(,{desc="desc",foodId=4,menuId=8589934594,menu__content="content",menu__food=["d"],menu__menuId=2,menu__status=0,name="d",price=5,status=2})"
      R"(,{desc="desc",foodId=5,menuId=3,menu__content="content",menu__food=["e"],menu__menuId=3,menu__status=2,name="e",price=10.5,status=2})"
      R"(,{desc="desc",foodId=6,menuId=4294967300,menu__content="content",menu__food=["f","g"],menu__menuId=4,menu__status=1,name="f",price=25.5,status=2})"
      R"(,{desc="desc",foodId=7,menuId=4294967300,menu__content="content",menu__food=["f","g"],menu__menuId=4,menu__status=1,name="g",price=1.5,status=2}])",
      (view | toCson()).c_str());

  EXPECT_STREQ(
      R"({data=)"
      R"([["desc",1,4294967297,"a",2,2,5.5,"content",1,["a","b","c"]])"
      R"(,["desc",2,4294967297,"b",2,2,4.5,"content",1,["a","b","c"]])"
      R"(,["desc",3,4294967297,"c",2,2,4.5,"content",1,["a","b","c"]])"
      R"(,["desc",4,8589934594,"d",2,0,5,"content",2,["d"]])"
      R"(,["desc",5,3,"e",2,2,10.5,"content",3,["e"]])"
      R"(,["desc",6,4294967300,"f",2,1,25.5,"content",4,["f","g"]])"
      R"(,["desc",7,4294967300,"g",2,1,1.5,"content",4,["f","g"]]])"
      R"(,name=["desc","foodId","menuId","name","status","menu__status","price","menu__content","menu__menuId","menu__food"])"
      R"(,type=["STRING","UINT64","UINT64","STRING","INT32","INT32","FLOAT","STRING","UINT64","STRING"]})",
      (view | toCson(true)).c_str());
}
