
#include "fuel_match.h"

namespace kitlus {

using pyne::simple_xs;

#define LG(X) LOG(cyclus::LEV_##X, "kitlus")

double CosiWeight(cyclus::Composition::Ptr c, std::string spectrum) {
  cyclus::CompMap cm = c->mass();
  cyclus::compmath::Normalize(&cm);

  double fiss_u238 = simple_xs("u238", "fission", spectrum);
  double absorb_u238 = simple_xs("u238", "absorption", spectrum);
  double nu_u238 = 0;
  double p_u238 = nu_u238 * fiss_u238 - absorb_u238;

  double fiss_pu239 = simple_xs("Pu239", "fission", spectrum);
  double absorb_pu239 = simple_xs("Pu239", "absorption", spectrum);
  double nu_pu239 = 2.85;
  double p_pu239 = nu_pu239 * fiss_pu239 - absorb_pu239;

  cyclus::CompMap::iterator it;
  double w = 0;
  for (it = cm.begin(); it != cm.end(); ++it) {
    cyclus::Nuc nuc = it->first;
    double nu = 0;
    if (nuc == 922350000) {
      nu = 2.4;
    } else if (nuc == 922330000) {
      nu = 2.5;
    } else if (nuc == 942390000) {
      nu = 2.85;
    }

    double fiss = 0;
    double absorb = 0;
    try {
      fiss = simple_xs(nuc, "fission", spectrum);
      absorb = simple_xs(nuc, "absorption", spectrum);
    } catch(pyne::InvalidSimpleXS err) { }

    double p = nu * fiss - absorb;
    w += it->second * (p - p_u238) / (p_pu239 - p_u238);
  }
  return w;
}

double CosiFissileFrac(cyclus::Composition::Ptr target,
                       cyclus::Composition::Ptr filler,
                       cyclus::Composition::Ptr fissile,
                       std::string spectrum) {
  double w_fill = CosiWeight(filler, spectrum);
  double w_fiss = CosiWeight(fissile, spectrum);
  double w_tgt = CosiWeight(target, spectrum);

  LG(INFO4) << "w_fill=" << w_fill;
  LG(INFO4) << "w_tgt=" << w_tgt;
  LG(INFO4) << "w_fiss=" << w_fiss;

  return (w_tgt - w_fill) / (w_fiss - w_fill);
}

} // namespace kitlus
