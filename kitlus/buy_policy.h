#ifndef KITLUS_BUY_POLICY_H_
#define KITLUS_BUY_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

class BuyPolicy : public cyclus::Trader {
 public:
  BuyPolicy(cyclus::Agent* manager) : cyclus::Trader(manager) {};

  virtual ~BuyPolicy() {};

  /// Init configures the policy to keep buf full every time step.  If
  /// quantize is greater than zero, the policy will make exclusive, integral
  /// quantize kg requests.  Otherwise, single requests will be sent to
  /// fill the buffer's empty space.
  void Init(cyclus::toolkit::ResourceBuff* buf, std::string name, double quantize = -1);

  /// Set configures the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can have an
  /// arbitrary number of policies set.  Recalling Set to modify the composition
  /// or preference of a commodity that has previously been set is allowed.
  void Set(std::string commod, cyclus::Composition::Ptr c, double pref = 0.0);

  /// Commods returns resources and their respective commodities that were
  /// received on the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  std::map<cyclus::Material::Ptr, std::string> Commods() {
    return rsrc_commod_;
  };

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
  };

  double quantize_;

  std::string name_;

  std::map<cyclus::Material::Ptr, std::string> rsrc_commod_;

  cyclus::toolkit::ResourceBuff* buf_;

  std::map<std::string, CommodDetail> commods_;
};

} // namespace kitlus

#endif
