#include "recipe_mixer.h"
#include "kitlus/fuel_match.h"

#define LG(X) LOG(cyclus::LEV_##X, "RecMix")

using cyclus::Material;
using cyclus::Composition;
using cyclus::ResCast;

RecipeMixer::RecipeMixer(cyclus::Context* ctx)
  : cyclus::Facility(ctx),
    fill_size_(0),
    fiss_size_(0),
    out_size_(0),
    throughput_(0) {}

void RecipeMixer::EnterNotify() {
  cyclus::Facility::EnterNotify();

  outpolicy_.Init(this, &out_, "outbuf").Set(outcommod_);
  fillpolicy_.Init(this, &fill_, "filler")
             .Set(fill_commod_, context()->GetRecipe(fill_recipe_));
  fisspolicy_.Init(this, &fiss_, "fissile")
             .Set(fiss_commod_, context()->GetRecipe(fiss_recipe_));

  context()->RegisterTrader(&outpolicy_);
  context()->RegisterTrader(&fillpolicy_);
  context()->RegisterTrader(&fisspolicy_);
}

void RecipeMixer::Tick() {
  LG(INFO3) << "RecipeMixer id=" << id() << " is ticking";
  LG(INFO4) << "filler quantity = " << fill_.quantity();
  LG(INFO4) << "fissile quantity = " << fiss_.quantity();
  LG(INFO4) << "outbuf quantity = " << out_.quantity();
  double qty = std::min(throughput_, out_.space());
  if (fill_.quantity() < cyclus::eps() || fiss_.quantity() < cyclus::eps() || qty < cyclus::eps()) {
    return;
  }

  // combine inbuf resources to single mats for querying
  std::vector<Material::Ptr> mats;
  mats = ResCast<Material>(fill_.PopN(fill_.count()));
  Material::Ptr m1 = mats[0];
  for (int i = 1; i < mats.size(); ++i) {
    m1->Absorb(mats[i]);
  }

  mats = ResCast<Material>(fiss_.PopN(fiss_.count()));
  Material::Ptr m2 = mats[0];
  for (int i = 1; i < mats.size(); ++i) {
    m2->Absorb(mats[i]);
  }

  // determine frac needed from each input stream
  Composition::Ptr tgt = context()->GetRecipe(outrecipe_);
  double frac2 = kitlus::CosiFissileFrac(tgt, m1->comp(), m2->comp());
  double frac1 = 1 - frac2;
  if (frac2 < 0) {
    fill_.Push(m1);
    fiss_.Push(m2);
    LG(ERROR) << "fiss stream has too low reactivity";
    return;
  }

  LG(INFO4) << "fill frac = " << frac1;
  LG(INFO4) << "fiss frac = " << frac2;

  // deal with stream quantity and out buf space constraints
  double qty1 = frac1 * qty;
  double qty2 = frac2 * qty;
  double qty1diff = m1->quantity() - qty1;
  double qty2diff = m2->quantity() - qty2;
  if (qty1diff >= 0 && qty2diff >= 0) {
    // not constrained by inbuf quantities
  } else if (qty1diff < qty2diff ) {
    // constrained by fiss_
    LG(INFO5) << "Constrained by incommod '" << fill_commod_
              << "' - reducing qty from " << qty
              << " to " << m1->quantity() / frac1;
    qty = m1->quantity() / frac1;
  } else {
    // constrained by inbuf2
    LG(INFO5) << "Constrained by incommod '" << fiss_commod_
              << "' - reducing qty from " << qty
              << " to " << m2->quantity() / frac2;
    qty = m2->quantity() / frac2;
  }

  Material::Ptr mix = m1->ExtractQty(std::min(frac1 * qty, m1->quantity()));
  mix->Absorb(m2->ExtractQty(std::min(frac2 * qty, m2->quantity())));

  cyclus::toolkit::MatQuery mq(mix);
  LG(INFO4) << "Mixed " << mix->quantity() << " kg to recipe";
  LG(INFO5) << " u238 = " << mq.mass_frac(922380000);
  LG(INFO5) << " u235 = " << mq.mass_frac(922350000);
  LG(INFO5) << "Pu239 = " << mq.mass_frac(942390000);

  out_.Push(mix);
  if (m1->quantity() > 0) {
    fill_.Push(m1);
  }
  if (m2->quantity() > 0) {
    fiss_.Push(m2);
  }
}

extern "C" cyclus::Agent* ConstructRecipeMixer(cyclus::Context* ctx) {
  return new RecipeMixer(ctx);
}



