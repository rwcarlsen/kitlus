#include "separator.h"
#include <iomanip>

#define LG(X) LOG(cyclus::LEV_##X, "Seprat")

using cyclus::toolkit::MatQuery;
using cyclus::Material;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::ResCast;
using cyclus::Nuc;
using cyclus::compmath::Normalize;

Separator::Separator(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      inbuf_size_(0),
      inpref_(0),
      outbuf_size_(0),
      wastebuf_size_(0),
      throughput_(0),
      inpolicy_(this),
      outpolicy_(this),
      wastepolicy_(this) {}

void Separator::EnterNotify() {
  cyclus::Facility::EnterNotify();

  outpolicy_.Init(&outbuf_, outcommod_);
  wastepolicy_.Init(&wastebuf_, wastecommod_);
  inpolicy_.Init(&inbuf_, incommod_, context()->GetRecipe(inrecipe_), inpref_);

  context()->RegisterTrader(&outpolicy_);
  context()->RegisterTrader(&wastepolicy_);
  context()->RegisterTrader(&inpolicy_);
}

void Separator::Decommission() {
  context()->UnregisterTrader(&outpolicy_);
  context()->UnregisterTrader(&wastepolicy_);
  context()->UnregisterTrader(&inpolicy_);
  cyclus::Facility::Decommission();
}

void Separator::Tick() {
  LG(INFO3) << "Separator id=" << id() << " is ticking";
  LG(INFO4) << "inbuf quantity = " << std::setprecision(17) << inbuf_.quantity();
  LG(INFO4) << "outbuf quantity = " << std::setprecision(17) << outbuf_.quantity();
  LG(INFO4) << "wastebuf quantity = " << std::setprecision(17) << wastebuf_.quantity();
  if (inbuf_.count() == 0) {
    return;
  }

  std::vector<Material::Ptr> mats;
  mats = ResCast<Material>(inbuf_.PopN(inbuf_.count()));
  for (int i = 1; i < mats.size(); ++i) {
    mats[0]->Absorb(mats[i]);
  }
  Material::Ptr m = mats[0];

  CompMap cm = m->comp()->mass();
  Normalize(&cm, m->quantity());
  CompMap::iterator it;
  double qty = 0;
  for (it = cm.begin(); it != cm.end(); ++it) {
    Nuc n = it->first;
    if (effs_.count(n) == 0) {
      cm[n] = 0;
    } else {
      cm[n] *= effs_[n];
      LG(INFO5) << "extracting qty " << std::setprecision(17) << cm[n] << " of nuc " << n << " (eff=" << effs_[n] << ")";
      qty += cm[n];
    }
  }

  LG(INFO4) << "extracting total qty " << std::setprecision(17) << qty << " from inbuf";

  Composition::Ptr c = Composition::CreateFromMass(cm);
  Material::Ptr keep = m->ExtractComp(std::min(qty, m->quantity()), c);

  if (m->quantity() <= wastebuf_.space()) {
    wastebuf_.Push(m);
  } else {
    wastebuf_.Push(m->ExtractQty(wastebuf_.space()));
    inbuf_.Push(m);
  }

  if (qty < throughput_ && qty < outbuf_.space()) {
    outbuf_.Push(keep);
  } else {
    qty = std::min(throughput_, outbuf_.space());
    outbuf_.Push(keep->ExtractQty(qty));
    inbuf_.Push(keep);
  }
}

extern "C" cyclus::Agent* ConstructSeparator(cyclus::Context* ctx) {
  return new Separator(ctx);
}
