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

/**
 * The CRYSTAL_NARG macro evaluates to the number of arguments that have been
 * passed to it.
 *
 * Laurent Deniau, "__VA_NARG__," 17 January 2006, <comp.std.c> (29 November 2007).
 */
#define CRYSTAL_NARG_(dummy,              \
  _99,_98,_97,_96,_95,_94,_93,_92,_91,_90,\
  _89,_88,_87,_86,_85,_84,_83,_82,_81,_80,\
  _79,_78,_77,_76,_75,_74,_73,_72,_71,_70,\
  _69,_68,_67,_66,_65,_64,_63,_62,_61,_60,\
  _59,_58,_57,_56,_55,_54,_53,_52,_51,_50,\
  _49,_48,_47,_46,_45,_44,_43,_42,_41,_40,\
  _39,_38,_37,_36,_35,_34,_33,_32,_31,_30,\
  _29,_28,_27,_26,_25,_24,_23,_22,_21,_20,\
  _19,_18,_17,_16,_15,_14,_13,_12,_11,_10,\
   _9, _8, _7, _6, _5, _4, _3, _2, _1, _0,...) _0

#define CRYSTAL_NARG(...)                 \
  CRYSTAL_NARG_(dummy, ##__VA_ARGS__,     \
  99, 98, 97, 96, 95, 94, 93, 92, 91, 90, \
  89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
  79, 78, 77, 76, 75, 74, 73, 72, 71, 70, \
  69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
  59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
  49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
  39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
  29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
  19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
   9,  8,  7,  6,  5,  4,  3,  2,  1,  0)

/**
 * APPLYXn variadic X-Macro.
 */
#define CRYSTAL_APPLYX0(X, ...)
#define CRYSTAL_APPLYX1(X, a, ...) X(a) CRYSTAL_APPLYX0(X, __VA_ARGS__)
#define CRYSTAL_APPLYX2(X, a, ...) X(a) CRYSTAL_APPLYX1(X, __VA_ARGS__)
#define CRYSTAL_APPLYX3(X, a, ...) X(a) CRYSTAL_APPLYX2(X, __VA_ARGS__)
#define CRYSTAL_APPLYX4(X, a, ...) X(a) CRYSTAL_APPLYX3(X, __VA_ARGS__)
#define CRYSTAL_APPLYX5(X, a, ...) X(a) CRYSTAL_APPLYX4(X, __VA_ARGS__)
#define CRYSTAL_APPLYX6(X, a, ...) X(a) CRYSTAL_APPLYX5(X, __VA_ARGS__)
#define CRYSTAL_APPLYX7(X, a, ...) X(a) CRYSTAL_APPLYX6(X, __VA_ARGS__)
#define CRYSTAL_APPLYX8(X, a, ...) X(a) CRYSTAL_APPLYX7(X, __VA_ARGS__)
#define CRYSTAL_APPLYX9(X, a, ...) X(a) CRYSTAL_APPLYX8(X, __VA_ARGS__)

#define CRYSTAL_APPLYX10(X, a, ...) X(a) CRYSTAL_APPLYX9(X, __VA_ARGS__)
#define CRYSTAL_APPLYX11(X, a, ...) X(a) CRYSTAL_APPLYX10(X, __VA_ARGS__)
#define CRYSTAL_APPLYX12(X, a, ...) X(a) CRYSTAL_APPLYX11(X, __VA_ARGS__)
#define CRYSTAL_APPLYX13(X, a, ...) X(a) CRYSTAL_APPLYX12(X, __VA_ARGS__)
#define CRYSTAL_APPLYX14(X, a, ...) X(a) CRYSTAL_APPLYX13(X, __VA_ARGS__)
#define CRYSTAL_APPLYX15(X, a, ...) X(a) CRYSTAL_APPLYX14(X, __VA_ARGS__)
#define CRYSTAL_APPLYX16(X, a, ...) X(a) CRYSTAL_APPLYX15(X, __VA_ARGS__)
#define CRYSTAL_APPLYX17(X, a, ...) X(a) CRYSTAL_APPLYX16(X, __VA_ARGS__)
#define CRYSTAL_APPLYX18(X, a, ...) X(a) CRYSTAL_APPLYX17(X, __VA_ARGS__)
#define CRYSTAL_APPLYX19(X, a, ...) X(a) CRYSTAL_APPLYX18(X, __VA_ARGS__)

#define CRYSTAL_APPLYX20(X, a, ...) X(a) CRYSTAL_APPLYX19(X, __VA_ARGS__)
#define CRYSTAL_APPLYX21(X, a, ...) X(a) CRYSTAL_APPLYX20(X, __VA_ARGS__)
#define CRYSTAL_APPLYX22(X, a, ...) X(a) CRYSTAL_APPLYX21(X, __VA_ARGS__)
#define CRYSTAL_APPLYX23(X, a, ...) X(a) CRYSTAL_APPLYX22(X, __VA_ARGS__)
#define CRYSTAL_APPLYX24(X, a, ...) X(a) CRYSTAL_APPLYX23(X, __VA_ARGS__)
#define CRYSTAL_APPLYX25(X, a, ...) X(a) CRYSTAL_APPLYX24(X, __VA_ARGS__)
#define CRYSTAL_APPLYX26(X, a, ...) X(a) CRYSTAL_APPLYX25(X, __VA_ARGS__)
#define CRYSTAL_APPLYX27(X, a, ...) X(a) CRYSTAL_APPLYX26(X, __VA_ARGS__)
#define CRYSTAL_APPLYX28(X, a, ...) X(a) CRYSTAL_APPLYX27(X, __VA_ARGS__)
#define CRYSTAL_APPLYX29(X, a, ...) X(a) CRYSTAL_APPLYX28(X, __VA_ARGS__)

#define CRYSTAL_APPLYX30(X, a, ...) X(a) CRYSTAL_APPLYX29(X, __VA_ARGS__)
#define CRYSTAL_APPLYX31(X, a, ...) X(a) CRYSTAL_APPLYX30(X, __VA_ARGS__)
#define CRYSTAL_APPLYX32(X, a, ...) X(a) CRYSTAL_APPLYX31(X, __VA_ARGS__)
#define CRYSTAL_APPLYX33(X, a, ...) X(a) CRYSTAL_APPLYX32(X, __VA_ARGS__)
#define CRYSTAL_APPLYX34(X, a, ...) X(a) CRYSTAL_APPLYX33(X, __VA_ARGS__)
#define CRYSTAL_APPLYX35(X, a, ...) X(a) CRYSTAL_APPLYX34(X, __VA_ARGS__)
#define CRYSTAL_APPLYX36(X, a, ...) X(a) CRYSTAL_APPLYX35(X, __VA_ARGS__)
#define CRYSTAL_APPLYX37(X, a, ...) X(a) CRYSTAL_APPLYX36(X, __VA_ARGS__)
#define CRYSTAL_APPLYX38(X, a, ...) X(a) CRYSTAL_APPLYX37(X, __VA_ARGS__)
#define CRYSTAL_APPLYX39(X, a, ...) X(a) CRYSTAL_APPLYX38(X, __VA_ARGS__)

#define CRYSTAL_APPLYX40(X, a, ...) X(a) CRYSTAL_APPLYX39(X, __VA_ARGS__)
#define CRYSTAL_APPLYX41(X, a, ...) X(a) CRYSTAL_APPLYX40(X, __VA_ARGS__)
#define CRYSTAL_APPLYX42(X, a, ...) X(a) CRYSTAL_APPLYX41(X, __VA_ARGS__)
#define CRYSTAL_APPLYX43(X, a, ...) X(a) CRYSTAL_APPLYX42(X, __VA_ARGS__)
#define CRYSTAL_APPLYX44(X, a, ...) X(a) CRYSTAL_APPLYX43(X, __VA_ARGS__)
#define CRYSTAL_APPLYX45(X, a, ...) X(a) CRYSTAL_APPLYX44(X, __VA_ARGS__)
#define CRYSTAL_APPLYX46(X, a, ...) X(a) CRYSTAL_APPLYX45(X, __VA_ARGS__)
#define CRYSTAL_APPLYX47(X, a, ...) X(a) CRYSTAL_APPLYX46(X, __VA_ARGS__)
#define CRYSTAL_APPLYX48(X, a, ...) X(a) CRYSTAL_APPLYX47(X, __VA_ARGS__)
#define CRYSTAL_APPLYX49(X, a, ...) X(a) CRYSTAL_APPLYX48(X, __VA_ARGS__)

#define CRYSTAL_APPLYX50(X, a, ...) X(a) CRYSTAL_APPLYX49(X, __VA_ARGS__)
#define CRYSTAL_APPLYX51(X, a, ...) X(a) CRYSTAL_APPLYX50(X, __VA_ARGS__)
#define CRYSTAL_APPLYX52(X, a, ...) X(a) CRYSTAL_APPLYX51(X, __VA_ARGS__)
#define CRYSTAL_APPLYX53(X, a, ...) X(a) CRYSTAL_APPLYX52(X, __VA_ARGS__)
#define CRYSTAL_APPLYX54(X, a, ...) X(a) CRYSTAL_APPLYX53(X, __VA_ARGS__)
#define CRYSTAL_APPLYX55(X, a, ...) X(a) CRYSTAL_APPLYX54(X, __VA_ARGS__)
#define CRYSTAL_APPLYX56(X, a, ...) X(a) CRYSTAL_APPLYX55(X, __VA_ARGS__)
#define CRYSTAL_APPLYX57(X, a, ...) X(a) CRYSTAL_APPLYX56(X, __VA_ARGS__)
#define CRYSTAL_APPLYX58(X, a, ...) X(a) CRYSTAL_APPLYX57(X, __VA_ARGS__)
#define CRYSTAL_APPLYX59(X, a, ...) X(a) CRYSTAL_APPLYX58(X, __VA_ARGS__)

#define CRYSTAL_APPLYX60(X, a, ...) X(a) CRYSTAL_APPLYX59(X, __VA_ARGS__)
#define CRYSTAL_APPLYX61(X, a, ...) X(a) CRYSTAL_APPLYX60(X, __VA_ARGS__)
#define CRYSTAL_APPLYX62(X, a, ...) X(a) CRYSTAL_APPLYX61(X, __VA_ARGS__)
#define CRYSTAL_APPLYX63(X, a, ...) X(a) CRYSTAL_APPLYX62(X, __VA_ARGS__)
#define CRYSTAL_APPLYX64(X, a, ...) X(a) CRYSTAL_APPLYX63(X, __VA_ARGS__)
#define CRYSTAL_APPLYX65(X, a, ...) X(a) CRYSTAL_APPLYX64(X, __VA_ARGS__)
#define CRYSTAL_APPLYX66(X, a, ...) X(a) CRYSTAL_APPLYX65(X, __VA_ARGS__)
#define CRYSTAL_APPLYX67(X, a, ...) X(a) CRYSTAL_APPLYX66(X, __VA_ARGS__)
#define CRYSTAL_APPLYX68(X, a, ...) X(a) CRYSTAL_APPLYX67(X, __VA_ARGS__)
#define CRYSTAL_APPLYX69(X, a, ...) X(a) CRYSTAL_APPLYX68(X, __VA_ARGS__)

#define CRYSTAL_APPLYX70(X, a, ...) X(a) CRYSTAL_APPLYX69(X, __VA_ARGS__)
#define CRYSTAL_APPLYX71(X, a, ...) X(a) CRYSTAL_APPLYX70(X, __VA_ARGS__)
#define CRYSTAL_APPLYX72(X, a, ...) X(a) CRYSTAL_APPLYX71(X, __VA_ARGS__)
#define CRYSTAL_APPLYX73(X, a, ...) X(a) CRYSTAL_APPLYX72(X, __VA_ARGS__)
#define CRYSTAL_APPLYX74(X, a, ...) X(a) CRYSTAL_APPLYX73(X, __VA_ARGS__)
#define CRYSTAL_APPLYX75(X, a, ...) X(a) CRYSTAL_APPLYX74(X, __VA_ARGS__)
#define CRYSTAL_APPLYX76(X, a, ...) X(a) CRYSTAL_APPLYX75(X, __VA_ARGS__)
#define CRYSTAL_APPLYX77(X, a, ...) X(a) CRYSTAL_APPLYX76(X, __VA_ARGS__)
#define CRYSTAL_APPLYX78(X, a, ...) X(a) CRYSTAL_APPLYX77(X, __VA_ARGS__)
#define CRYSTAL_APPLYX79(X, a, ...) X(a) CRYSTAL_APPLYX78(X, __VA_ARGS__)

#define CRYSTAL_APPLYX80(X, a, ...) X(a) CRYSTAL_APPLYX79(X, __VA_ARGS__)
#define CRYSTAL_APPLYX81(X, a, ...) X(a) CRYSTAL_APPLYX80(X, __VA_ARGS__)
#define CRYSTAL_APPLYX82(X, a, ...) X(a) CRYSTAL_APPLYX81(X, __VA_ARGS__)
#define CRYSTAL_APPLYX83(X, a, ...) X(a) CRYSTAL_APPLYX82(X, __VA_ARGS__)
#define CRYSTAL_APPLYX84(X, a, ...) X(a) CRYSTAL_APPLYX83(X, __VA_ARGS__)
#define CRYSTAL_APPLYX85(X, a, ...) X(a) CRYSTAL_APPLYX84(X, __VA_ARGS__)
#define CRYSTAL_APPLYX86(X, a, ...) X(a) CRYSTAL_APPLYX85(X, __VA_ARGS__)
#define CRYSTAL_APPLYX87(X, a, ...) X(a) CRYSTAL_APPLYX86(X, __VA_ARGS__)
#define CRYSTAL_APPLYX88(X, a, ...) X(a) CRYSTAL_APPLYX87(X, __VA_ARGS__)
#define CRYSTAL_APPLYX89(X, a, ...) X(a) CRYSTAL_APPLYX88(X, __VA_ARGS__)

#define CRYSTAL_APPLYX90(X, a, ...) X(a) CRYSTAL_APPLYX89(X, __VA_ARGS__)
#define CRYSTAL_APPLYX91(X, a, ...) X(a) CRYSTAL_APPLYX90(X, __VA_ARGS__)
#define CRYSTAL_APPLYX92(X, a, ...) X(a) CRYSTAL_APPLYX91(X, __VA_ARGS__)
#define CRYSTAL_APPLYX93(X, a, ...) X(a) CRYSTAL_APPLYX92(X, __VA_ARGS__)
#define CRYSTAL_APPLYX94(X, a, ...) X(a) CRYSTAL_APPLYX93(X, __VA_ARGS__)
#define CRYSTAL_APPLYX95(X, a, ...) X(a) CRYSTAL_APPLYX94(X, __VA_ARGS__)
#define CRYSTAL_APPLYX96(X, a, ...) X(a) CRYSTAL_APPLYX95(X, __VA_ARGS__)
#define CRYSTAL_APPLYX97(X, a, ...) X(a) CRYSTAL_APPLYX96(X, __VA_ARGS__)
#define CRYSTAL_APPLYX98(X, a, ...) X(a) CRYSTAL_APPLYX97(X, __VA_ARGS__)
#define CRYSTAL_APPLYX99(X, a, ...) X(a) CRYSTAL_APPLYX98(X, __VA_ARGS__)

#define CRYSTAL_APPLYXi(X, n, ...) CRYSTAL_APPLYX##n(X, __VA_ARGS__)
#define CRYSTAL_APPLYX_(X, n, ...) CRYSTAL_APPLYXi(X, n, __VA_ARGS__)

#define CRYSTAL_APPLYXn(X, ...) \
  CRYSTAL_APPLYX_(X, CRYSTAL_NARG(__VA_ARGS__), __VA_ARGS__)

/**
 * APPLYFn variadic F-Macro.
 */
#define CRYSTAL_APPLYF0(x, ...)
#define CRYSTAL_APPLYF1(x, A, ...) A(x) CRYSTAL_APPLYF0(x, __VA_ARGS__)
#define CRYSTAL_APPLYF2(x, A, ...) A(x) CRYSTAL_APPLYF1(x, __VA_ARGS__)
#define CRYSTAL_APPLYF3(x, A, ...) A(x) CRYSTAL_APPLYF2(x, __VA_ARGS__)
#define CRYSTAL_APPLYF4(x, A, ...) A(x) CRYSTAL_APPLYF3(x, __VA_ARGS__)
#define CRYSTAL_APPLYF5(x, A, ...) A(x) CRYSTAL_APPLYF4(x, __VA_ARGS__)
#define CRYSTAL_APPLYF6(x, A, ...) A(x) CRYSTAL_APPLYF5(x, __VA_ARGS__)
#define CRYSTAL_APPLYF7(x, A, ...) A(x) CRYSTAL_APPLYF6(x, __VA_ARGS__)
#define CRYSTAL_APPLYF8(x, A, ...) A(x) CRYSTAL_APPLYF7(x, __VA_ARGS__)
#define CRYSTAL_APPLYF9(x, A, ...) A(x) CRYSTAL_APPLYF8(x, __VA_ARGS__)

#define CRYSTAL_APPLYF10(x, A, ...) A(x) CRYSTAL_APPLYF9(x, __VA_ARGS__)
#define CRYSTAL_APPLYF11(x, A, ...) A(x) CRYSTAL_APPLYF10(x, __VA_ARGS__)
#define CRYSTAL_APPLYF12(x, A, ...) A(x) CRYSTAL_APPLYF11(x, __VA_ARGS__)
#define CRYSTAL_APPLYF13(x, A, ...) A(x) CRYSTAL_APPLYF12(x, __VA_ARGS__)
#define CRYSTAL_APPLYF14(x, A, ...) A(x) CRYSTAL_APPLYF13(x, __VA_ARGS__)
#define CRYSTAL_APPLYF15(x, A, ...) A(x) CRYSTAL_APPLYF14(x, __VA_ARGS__)
#define CRYSTAL_APPLYF16(x, A, ...) A(x) CRYSTAL_APPLYF15(x, __VA_ARGS__)
#define CRYSTAL_APPLYF17(x, A, ...) A(x) CRYSTAL_APPLYF16(x, __VA_ARGS__)
#define CRYSTAL_APPLYF18(x, A, ...) A(x) CRYSTAL_APPLYF17(x, __VA_ARGS__)
#define CRYSTAL_APPLYF19(x, A, ...) A(x) CRYSTAL_APPLYF18(x, __VA_ARGS__)

#define CRYSTAL_APPLYF20(x, A, ...) A(x) CRYSTAL_APPLYF19(x, __VA_ARGS__)
#define CRYSTAL_APPLYF21(x, A, ...) A(x) CRYSTAL_APPLYF20(x, __VA_ARGS__)
#define CRYSTAL_APPLYF22(x, A, ...) A(x) CRYSTAL_APPLYF21(x, __VA_ARGS__)
#define CRYSTAL_APPLYF23(x, A, ...) A(x) CRYSTAL_APPLYF22(x, __VA_ARGS__)
#define CRYSTAL_APPLYF24(x, A, ...) A(x) CRYSTAL_APPLYF23(x, __VA_ARGS__)
#define CRYSTAL_APPLYF25(x, A, ...) A(x) CRYSTAL_APPLYF24(x, __VA_ARGS__)
#define CRYSTAL_APPLYF26(x, A, ...) A(x) CRYSTAL_APPLYF25(x, __VA_ARGS__)
#define CRYSTAL_APPLYF27(x, A, ...) A(x) CRYSTAL_APPLYF26(x, __VA_ARGS__)
#define CRYSTAL_APPLYF28(x, A, ...) A(x) CRYSTAL_APPLYF27(x, __VA_ARGS__)
#define CRYSTAL_APPLYF29(x, A, ...) A(x) CRYSTAL_APPLYF28(x, __VA_ARGS__)

#define CRYSTAL_APPLYF30(x, A, ...) A(x) CRYSTAL_APPLYF29(x, __VA_ARGS__)
#define CRYSTAL_APPLYF31(x, A, ...) A(x) CRYSTAL_APPLYF30(x, __VA_ARGS__)
#define CRYSTAL_APPLYF32(x, A, ...) A(x) CRYSTAL_APPLYF31(x, __VA_ARGS__)
#define CRYSTAL_APPLYF33(x, A, ...) A(x) CRYSTAL_APPLYF32(x, __VA_ARGS__)
#define CRYSTAL_APPLYF34(x, A, ...) A(x) CRYSTAL_APPLYF33(x, __VA_ARGS__)
#define CRYSTAL_APPLYF35(x, A, ...) A(x) CRYSTAL_APPLYF34(x, __VA_ARGS__)
#define CRYSTAL_APPLYF36(x, A, ...) A(x) CRYSTAL_APPLYF35(x, __VA_ARGS__)
#define CRYSTAL_APPLYF37(x, A, ...) A(x) CRYSTAL_APPLYF36(x, __VA_ARGS__)
#define CRYSTAL_APPLYF38(x, A, ...) A(x) CRYSTAL_APPLYF37(x, __VA_ARGS__)
#define CRYSTAL_APPLYF39(x, A, ...) A(x) CRYSTAL_APPLYF38(x, __VA_ARGS__)

#define CRYSTAL_APPLYF40(x, A, ...) A(x) CRYSTAL_APPLYF39(x, __VA_ARGS__)
#define CRYSTAL_APPLYF41(x, A, ...) A(x) CRYSTAL_APPLYF40(x, __VA_ARGS__)
#define CRYSTAL_APPLYF42(x, A, ...) A(x) CRYSTAL_APPLYF41(x, __VA_ARGS__)
#define CRYSTAL_APPLYF43(x, A, ...) A(x) CRYSTAL_APPLYF42(x, __VA_ARGS__)
#define CRYSTAL_APPLYF44(x, A, ...) A(x) CRYSTAL_APPLYF43(x, __VA_ARGS__)
#define CRYSTAL_APPLYF45(x, A, ...) A(x) CRYSTAL_APPLYF44(x, __VA_ARGS__)
#define CRYSTAL_APPLYF46(x, A, ...) A(x) CRYSTAL_APPLYF45(x, __VA_ARGS__)
#define CRYSTAL_APPLYF47(x, A, ...) A(x) CRYSTAL_APPLYF46(x, __VA_ARGS__)
#define CRYSTAL_APPLYF48(x, A, ...) A(x) CRYSTAL_APPLYF47(x, __VA_ARGS__)
#define CRYSTAL_APPLYF49(x, A, ...) A(x) CRYSTAL_APPLYF48(x, __VA_ARGS__)

#define CRYSTAL_APPLYF50(x, A, ...) A(x) CRYSTAL_APPLYF49(x, __VA_ARGS__)
#define CRYSTAL_APPLYF51(x, A, ...) A(x) CRYSTAL_APPLYF50(x, __VA_ARGS__)
#define CRYSTAL_APPLYF52(x, A, ...) A(x) CRYSTAL_APPLYF51(x, __VA_ARGS__)
#define CRYSTAL_APPLYF53(x, A, ...) A(x) CRYSTAL_APPLYF52(x, __VA_ARGS__)
#define CRYSTAL_APPLYF54(x, A, ...) A(x) CRYSTAL_APPLYF53(x, __VA_ARGS__)
#define CRYSTAL_APPLYF55(x, A, ...) A(x) CRYSTAL_APPLYF54(x, __VA_ARGS__)
#define CRYSTAL_APPLYF56(x, A, ...) A(x) CRYSTAL_APPLYF55(x, __VA_ARGS__)
#define CRYSTAL_APPLYF57(x, A, ...) A(x) CRYSTAL_APPLYF56(x, __VA_ARGS__)
#define CRYSTAL_APPLYF58(x, A, ...) A(x) CRYSTAL_APPLYF57(x, __VA_ARGS__)
#define CRYSTAL_APPLYF59(x, A, ...) A(x) CRYSTAL_APPLYF58(x, __VA_ARGS__)

#define CRYSTAL_APPLYF60(x, A, ...) A(x) CRYSTAL_APPLYF59(x, __VA_ARGS__)
#define CRYSTAL_APPLYF61(x, A, ...) A(x) CRYSTAL_APPLYF60(x, __VA_ARGS__)
#define CRYSTAL_APPLYF62(x, A, ...) A(x) CRYSTAL_APPLYF61(x, __VA_ARGS__)
#define CRYSTAL_APPLYF63(x, A, ...) A(x) CRYSTAL_APPLYF62(x, __VA_ARGS__)
#define CRYSTAL_APPLYF64(x, A, ...) A(x) CRYSTAL_APPLYF63(x, __VA_ARGS__)
#define CRYSTAL_APPLYF65(x, A, ...) A(x) CRYSTAL_APPLYF64(x, __VA_ARGS__)
#define CRYSTAL_APPLYF66(x, A, ...) A(x) CRYSTAL_APPLYF65(x, __VA_ARGS__)
#define CRYSTAL_APPLYF67(x, A, ...) A(x) CRYSTAL_APPLYF66(x, __VA_ARGS__)
#define CRYSTAL_APPLYF68(x, A, ...) A(x) CRYSTAL_APPLYF67(x, __VA_ARGS__)
#define CRYSTAL_APPLYF69(x, A, ...) A(x) CRYSTAL_APPLYF68(x, __VA_ARGS__)

#define CRYSTAL_APPLYF70(x, A, ...) A(x) CRYSTAL_APPLYF69(x, __VA_ARGS__)
#define CRYSTAL_APPLYF71(x, A, ...) A(x) CRYSTAL_APPLYF70(x, __VA_ARGS__)
#define CRYSTAL_APPLYF72(x, A, ...) A(x) CRYSTAL_APPLYF71(x, __VA_ARGS__)
#define CRYSTAL_APPLYF73(x, A, ...) A(x) CRYSTAL_APPLYF72(x, __VA_ARGS__)
#define CRYSTAL_APPLYF74(x, A, ...) A(x) CRYSTAL_APPLYF73(x, __VA_ARGS__)
#define CRYSTAL_APPLYF75(x, A, ...) A(x) CRYSTAL_APPLYF74(x, __VA_ARGS__)
#define CRYSTAL_APPLYF76(x, A, ...) A(x) CRYSTAL_APPLYF75(x, __VA_ARGS__)
#define CRYSTAL_APPLYF77(x, A, ...) A(x) CRYSTAL_APPLYF76(x, __VA_ARGS__)
#define CRYSTAL_APPLYF78(x, A, ...) A(x) CRYSTAL_APPLYF77(x, __VA_ARGS__)
#define CRYSTAL_APPLYF79(x, A, ...) A(x) CRYSTAL_APPLYF78(x, __VA_ARGS__)

#define CRYSTAL_APPLYF80(x, A, ...) A(x) CRYSTAL_APPLYF79(x, __VA_ARGS__)
#define CRYSTAL_APPLYF81(x, A, ...) A(x) CRYSTAL_APPLYF80(x, __VA_ARGS__)
#define CRYSTAL_APPLYF82(x, A, ...) A(x) CRYSTAL_APPLYF81(x, __VA_ARGS__)
#define CRYSTAL_APPLYF83(x, A, ...) A(x) CRYSTAL_APPLYF82(x, __VA_ARGS__)
#define CRYSTAL_APPLYF84(x, A, ...) A(x) CRYSTAL_APPLYF83(x, __VA_ARGS__)
#define CRYSTAL_APPLYF85(x, A, ...) A(x) CRYSTAL_APPLYF84(x, __VA_ARGS__)
#define CRYSTAL_APPLYF86(x, A, ...) A(x) CRYSTAL_APPLYF85(x, __VA_ARGS__)
#define CRYSTAL_APPLYF87(x, A, ...) A(x) CRYSTAL_APPLYF86(x, __VA_ARGS__)
#define CRYSTAL_APPLYF88(x, A, ...) A(x) CRYSTAL_APPLYF87(x, __VA_ARGS__)
#define CRYSTAL_APPLYF89(x, A, ...) A(x) CRYSTAL_APPLYF88(x, __VA_ARGS__)

#define CRYSTAL_APPLYF90(x, A, ...) A(x) CRYSTAL_APPLYF89(x, __VA_ARGS__)
#define CRYSTAL_APPLYF91(x, A, ...) A(x) CRYSTAL_APPLYF90(x, __VA_ARGS__)
#define CRYSTAL_APPLYF92(x, A, ...) A(x) CRYSTAL_APPLYF91(x, __VA_ARGS__)
#define CRYSTAL_APPLYF93(x, A, ...) A(x) CRYSTAL_APPLYF92(x, __VA_ARGS__)
#define CRYSTAL_APPLYF94(x, A, ...) A(x) CRYSTAL_APPLYF93(x, __VA_ARGS__)
#define CRYSTAL_APPLYF95(x, A, ...) A(x) CRYSTAL_APPLYF94(x, __VA_ARGS__)
#define CRYSTAL_APPLYF96(x, A, ...) A(x) CRYSTAL_APPLYF95(x, __VA_ARGS__)
#define CRYSTAL_APPLYF97(x, A, ...) A(x) CRYSTAL_APPLYF96(x, __VA_ARGS__)
#define CRYSTAL_APPLYF98(x, A, ...) A(x) CRYSTAL_APPLYF97(x, __VA_ARGS__)
#define CRYSTAL_APPLYF99(x, A, ...) A(x) CRYSTAL_APPLYF98(x, __VA_ARGS__)

#define CRYSTAL_APPLYFi(x, n, ...) CRYSTAL_APPLYF##n(x, __VA_ARGS__)
#define CRYSTAL_APPLYF_(x, n, ...) CRYSTAL_APPLYFi(x, n, __VA_ARGS__)

#define CRYSTAL_APPLYFn(x, ...) \
  CRYSTAL_APPLYF_(x, CRYSTAL_NARG(__VA_ARGS__), __VA_ARGS__)

