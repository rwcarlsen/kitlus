#ifndef RECIPE_MIXER_H_
#define RECIPE_MIXER_H_

#include <string>
#include "cyclus.h"
#include "kitlus/sell_policy.h"
#include "kitlus/buy_policy.h"

using kitlus::BuyPolicy;
using kitlus::SellPolicy;

class RecipeMixer : public cyclus::Facility {
 public:
  RecipeMixer(cyclus::Context* ctx);
  virtual ~RecipeMixer() {};
  virtual std::string str() {return "";};

  virtual void EnterNotify();
  virtual void Decommission();

  #pragma cyclus

  virtual void Tick();
  virtual void Tock() {};

  double Weight(cyclus::Composition::Ptr c);

 private:
  #pragma cyclus var {}
  std::string fill_commod_;
  #pragma cyclus var {}
  std::string fill_recipe_;
  #pragma cyclus var {}
  double fill_size_;
  #pragma cyclus var {'capacity': 'fill_size_'}
  cyclus::toolkit::ResourceBuff fill_;

  #pragma cyclus var {}
  std::string fiss_commod_;
  #pragma cyclus var {}
  std::string fiss_recipe_;
  #pragma cyclus var {}
  double fiss_size_;
  #pragma cyclus var {'capacity': 'fiss_size_'}
  cyclus::toolkit::ResourceBuff fiss_;

  #pragma cyclus var {}
  std::string outcommod_;
  #pragma cyclus var {}
  std::string outrecipe_;
  #pragma cyclus var {}
  double out_size_;
  #pragma cyclus var {'capacity': 'out_size_'}
  cyclus::toolkit::ResourceBuff out_;

  #pragma cyclus var {}
  double throughput_;

  SellPolicy outpolicy_;
  BuyPolicy fillpolicy_;
  BuyPolicy fisspolicy_;
};

#endif  // RECIPE_MIXER_H_
