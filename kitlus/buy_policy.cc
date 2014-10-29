#include "buy_policy.h"

using cyclus::CapacityConstraint;
using cyclus::Material;
using cyclus::RequestPortfolio;
using cyclus::Trade;
using cyclus::Bid;
using cyclus::Request;
using cyclus::PrefMap;

namespace kitlus {

void BuyPolicy::Init(cyclus::toolkit::ResourceBuff* buf, double quantize) {
  buf_ = buf;
  quantize_ = quantize;
}

void BuyPolicy::Set(std::string commod, cyclus::Composition::Ptr c, double pref) {
  CommodDetail d;
  d.comp = c;
  d.pref = pref;
  commods_[commod] = d;
}

std::set<RequestPortfolio<Material>::Ptr>
BuyPolicy::GetMatlRequests() {
  rsrc_commod_.clear();
  std::set<RequestPortfolio<Material>::Ptr> ports;
  double amt = buf_->space();
  if (amt < cyclus::eps()) {
    return ports;
  }

  double quanta = quantize_;
  bool exclusive = true;
  if (quantize_ < 0) {
    exclusive = false;
    quanta = amt;
  }

  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  std::map<std::string, CommodDetail>::iterator it;
  for (it = commods_.begin(); it != commods_.end(); ++it) {
    std::string commod = it->first;
    CommodDetail d = it->second;
    for (int i = 0; i < amt / quanta; i++) {
      Material::Ptr m = Material::CreateUntracked(quanta, d.comp);
      port->AddRequest(m, this, commod, exclusive);
    }
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
  rsrc_commod_.clear();
  for (it = resps.begin(); it != resps.end(); ++it) {
    rsrc_commod_[it->second] = it->first.request->commodity();
    buf_->Push(it->second);
  }
}

void BuyPolicy::AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
  PrefMap<Material>::type::iterator it;
  for (it = prefs.begin(); it != prefs.end(); ++it) {
    Request<Material>* r = it->first;
    double pref = commods_[r->commodity()].pref;
    std::map<Bid<Material>*, double>::iterator it2;
    std::map<Bid<Material>*, double> bids = it->second;
    for (it2 = bids.begin(); it2 != bids.end(); ++it2) {
      Bid<Material>* b = it2->first;
      prefs[r][b] = pref;
    }
  }
}

} // namespace kitlus
