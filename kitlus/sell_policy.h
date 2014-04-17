#ifndef KITLUS_SELL_POLICY_H_
#define KITLUS_SELL_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

class SellPolicy : public cyclus::Trader {
 public:
  SellPolicy(cyclus::Agent* manager) : Trader(manager) {};

  void Init(cyclus::ResourceBuff* buf, std::string commod);

  virtual ~SellPolicy() {};

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
  GetMatlBids(const cyclus::CommodMap<cyclus::Material>::type&
              commod_requests);

  virtual void GetMatlTrades(
    const std::vector< cyclus::Trade<cyclus::Material> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses);

 private:
  cyclus::ResourceBuff* buf_;
  std::string commod_;
};

} // namespace kitlus

#endif
