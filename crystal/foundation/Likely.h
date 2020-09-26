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

#if __GNUC__
#define CRYSTAL_DETAIL_BUILTIN_EXPECT(b, t) (__builtin_expect(b, t))
#else
#define CRYSTAL_DETAIL_BUILTIN_EXPECT(b, t) b
#endif

#define CRYSTAL_LIKELY(x) CRYSTAL_DETAIL_BUILTIN_EXPECT((x), 1)
#define CRYSTAL_UNLIKELY(x) CRYSTAL_DETAIL_BUILTIN_EXPECT((x), 0)

