#pragma once

#include "crystal/foundation/Exception.h"

#define TF_THROW(...) CRYSTAL_THROW(crystal::RuntimeError, __VA_ARGS__)

