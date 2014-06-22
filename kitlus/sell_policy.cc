#include "sell_policy.h"

using cyclus::BidPortfolio;
using cyclus::CapacityConstraint;
using cyclus::Material;
using cyclus::Request;
using cyclus::Trade;
using cyclus::toolkit::Manifest;

#define LG(X) LOG(cyclus::LEV_##X, "kitlus")

namespace kitlus {


void SellPolicy::Init(cyclus::toolkit::ResourceBuff* buf, std::string commod) {
  buf_ = buf;
  commod_ = commod;
}

std::set<BidPortfolio<Material>::Ptr>
SellPolicy::GetMatlBids(cyclus::CommodMap<Material>::type& commod_requests) {
  std::set<BidPortfolio<Material>::Ptr> ports;
  if (buf_->empty()) {
    return ports;
  } else if (buf_->quantity() < cyclus::eps()) {
    return ports;
  } else if (commod_requests.count(commod_) < 1) {
    return ports;
  } 

  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

  const std::vector<Request<Material>*>& requests = commod_requests.at(
                                                          commod_);

  std::vector<Request<Material>*>::const_iterator it;
  for (it = requests.begin(); it != requests.end(); ++it) {
    Request<Material>* req = *it;
    double qty = std::min(req->target()->quantity(), buf_->capacity());
    Material::Ptr m = buf_->Pop<Material>();
    buf_->Push(m);
    Material::Ptr offer = Material::CreateUntracked(qty, m->comp());
    port->AddBid(req, offer, this);
  }

  LG(INFO5) << "SellPolicy for " << manager()->prototype() << ":" << manager()->id() << " offers " << buf_->quantity() << " kg of commod " << commod_;

  CapacityConstraint<Material> cc(buf_->quantity());
  port->AddConstraint(cc);
  ports.insert(port);
  return ports;
}

void SellPolicy::GetMatlTrades(
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>,
    Material::Ptr> >& responses) {

  std::vector< Trade<Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    LG(INFO5) << "SellPolicy for " << manager()->prototype() << ":" << manager()->id() << " filling order of " << qty << " kg";
    std::vector<Material::Ptr> man = cyclus::ResCast<Material>(buf_->PopQty(qty, buf_->quantity() * 1e-12));
    for (int i = 1; i < man.size(); ++i) {
      man[0]->Absorb(man[i]);
    }
    responses.push_back(std::make_pair(*it, man[0]));
  }
}

} // namespace kitlus
