#ifndef KITLUS_FUEL_MATCH_H_
#define KITLUS_FUEL_MATCH_H_

#include "cyclus.h"

namespace kitlus {

// spectrum must be one of "thermal" or "fission" (for fast spectrum calcs).
double CosiWeight(cyclus::Composition::Ptr c, std::string spectrum = "thermal");

// Returns the mass fraction of the fissile composition that should be mixed
// with "1 - frac" of the filler composition. In order to hit the target
// as close as possible.  spectrum must be one of "thermal" or "fission" (for
// fast spectrum calcs).
double CosiFissileFrac(cyclus::Composition::Ptr target,
                       cyclus::Composition::Ptr filler,
                       cyclus::Composition::Ptr fissile
                       std::string spectrum = "thermal");

} // namespace kitlus

#endif
