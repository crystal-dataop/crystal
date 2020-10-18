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

/*
 * Modified from folly.
 */

#pragma once

/**
 * CRYSTAL_ONE_OR_NONE(hello, world) expands to hello and
 * CRYSTAL_ONE_OR_NONE(hello) expands to nothing. This macro is used to
 * insert or eliminate text based on the presence of another argument.
 */
#define CRYSTAL_ONE_OR_NONE(a, ...) CRYSTAL_THIRD(a, ## __VA_ARGS__, a)
#define CRYSTAL_THIRD(a, b, ...) __VA_ARGS__

/**
 * Helper macro that extracts the second argument out of a list of any
 * number of arguments. If only one argument is given, it returns
 * that.
 */
#define CRYSTAL_ARG_2_OR_1(...) CR_ARG_2_OR_1_IMPL(__VA_ARGS__, __VA_ARGS__)
// Support macro for the above
#define CR_ARG_2_OR_1_IMPL(a, b, ...) b

/**
 * CRYSTAL_ANONYMOUS_VARIABLE(str) introduces an identifier starting with
 * str and ending with a number that varies with the line.
 */
#ifndef CRYSTAL_ANONYMOUS_VARIABLE
#define CRYSTAL_CONCATENATE_IMPL(s1, s2) s1##s2
#define CRYSTAL_CONCATENATE(s1, s2) CRYSTAL_CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define CRYSTAL_ANONYMOUS_VARIABLE(str) CRYSTAL_CONCATENATE(str, __COUNTER__)
#else
#define CRYSTAL_ANONYMOUS_VARIABLE(str) CRYSTAL_CONCATENATE(str, __LINE__)
#endif
#endif

/**
 * Use CRYSTAL_STRINGIZE(x) when you'd want to do what #x does inside
 * another macro expansion.
 */
#define CRYSTAL_STRINGIZE(x) #x
