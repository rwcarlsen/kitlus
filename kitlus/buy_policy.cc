#include "buy_policy.h"

using cyclus::CapacityConstraint;
using cyclus::Material;
using cyclus::RequestPortfolio;
using cyclus::Trade;
using cyclus::Bid;
using cyclus::Request;
using cyclus::PrefMap;

namespace kitlus {

void BuyPolicy::Init(cyclus::toolkit::ResourceBuff* buf) : buf_(buf){ }

void BuyPolicy::AddCommod(std::string commod, cyclus::Composition::Ptr c, double pref = 0.0) {
  CommodDetail d;
  d.comp = c;
  d.pref = pref;
  commods_[commod] = d;
}

std::set<RequestPortfolio<Material>::Ptr>
BuyPolicy::GetMatlRequests() {
  std::set<RequestPortfolio<Material>::Ptr> ports;
  double amt = buf_->space();
  if (amt < cyclus::eps()) {
    return ports;
  }

  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  std::map<std::string, CommodDetail>::iterator it;
  for (it = commods_.begin(); it != commods_.end(); ++it) {
    std::string commod = it->first;
    CommodDetail d = it->second;
    Material::Ptr m = Material::CreateUntracked(amt, d.comp);
    port->AddRequest(m, this, commod);
  }

  ports.insert(port);
  CapacityConstraint<Material> cc(amt);
  port->AddConstraint(cc);

  return ports;
}

void BuyPolicy::AcceptMatlTrades(
  const std::vector< std::pair<Trade<Material>,
  Material::Ptr> >& resps) {
  std::vector< std::pair<Trade<Material>, Material::Ptr> >::const_iterator it;
  for (it = resps.begin(); it != resps.end(); ++it) {
    buf_->Push(it->second);
  }
}

void BuyPolicy::AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
  PrefMap<Material>::type::iterator it;
  for (it = prefs.begin(); it != prefs.end(); ++it) {
    Request<Material>* r = it->first;
    commods_[r->commodity()]
    std::map<Bid<Material>*, double>::iterator it2;
    std::map<Bid<Material>*, double> bids = it->second;
    for (it2 = bids.begin(); it2 != bids.end(); ++it2) {
      Bid<Material>* b = it2->first;
      prefs[r][b] = pref_;
    }
  }
}

} // namespace kitlus
