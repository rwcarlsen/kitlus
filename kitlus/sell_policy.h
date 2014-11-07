#ifndef KITLUS_SELL_POLICY_H_
#define KITLUS_SELL_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

class SellPolicy : public cyclus::Trader {
 public:
  SellPolicy() : cyclus::Trader(NULL){};

  SellPolicy& Init(cyclus::Agent* manager, cyclus::toolkit::ResourceBuff* buf,
                   std::string name, double quantize = -1);

  SellPolicy& Set(std::string commod);

  virtual ~SellPolicy();

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr> GetMatlBids(
      cyclus::CommodMap<cyclus::Material>::type& commod_requests);

  virtual void GetMatlTrades(
      const std::vector<cyclus::Trade<cyclus::Material> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                            cyclus::Material::Ptr> >& responses);

 private:
  cyclus::toolkit::ResourceBuff* buf_;
  std::set<std::string> commods_;
  double quantize_;
  std::string name_;
};

}  // namespace kitlus

#endif
