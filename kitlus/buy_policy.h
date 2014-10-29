#ifndef KITLUS_BUY_POLICY_H_
#define KITLUS_BUY_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

class BuyPolicy : public cyclus::Trader {
 public:
  BuyPolicy(cyclus::Agent* manager) : cyclus::Trader(manager) {};

  virtual ~BuyPolicy() {};

  void Init(cyclus::toolkit::ResourceBuff* buf)
  void AddCommod(std::string commod, cyclus::Composition::Ptr c, double pref = 0.0);

  std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

  void AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& resps);

  virtual void AdjustMatlPrefs(cyclus::PrefMap<cyclus::Material>::type& prefs);

 private:
  struct CommodDetail {
    cyclus::Composition::Ptr comp;
    double pref;
  }

  cyclus::toolkit::ResourceBuff* buf_;

  std::map<std::string, CommodDetail> commods_;
};

} // namespace kitlus

#endif
