#ifndef KITLUS_BUY_POLICY_H_
#define KITLUS_BUY_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

class BuyPolicy : public cyclus::Trader {
 public:
  BuyPolicy(cyclus::Agent* manager) : cyclus::Trader(manager) {};

  virtual ~BuyPolicy() {};

  void Init(cyclus::toolkit::ResourceBuff* buf, std::string commod,
            cyclus::Composition::Ptr c, double pref = 0.0);

  std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

  void AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& resps);

  virtual void AdjustMatlPrefs(cyclus::PrefMap<cyclus::Material>::type& prefs);

 private:
  cyclus::toolkit::ResourceBuff* buf_;
  cyclus::Composition::Ptr comp_;
  std::string commod_;
  double pref_;
};

} // namespace kitlus

#endif
