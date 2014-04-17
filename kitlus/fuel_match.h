#ifndef KITLUS_FUEL_MATCH_H_
#define KITLUS_FUEL_MATCH_H_

#include "cyclus.h"

namespace kitlus {

double CosiWeight(cyclus::Composition::Ptr c);

// Returns the mass fraction of the fissile composition that should be mixed
// with "1 - frac" of the filler composition. In order to hit the target
// as close as possible.
double CosiFissileFrac(cyclus::Composition::Ptr target,
                       cyclus::Composition::Ptr filler,
                       cyclus::Composition::Ptr fissile);

} // namespace kitlus

#endif
