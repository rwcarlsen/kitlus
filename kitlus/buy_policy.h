#ifndef KITLUS_BUY_POLICY_H_
#define KITLUS_BUY_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

/// BuyPolicy performs semi-automatic inventory management of a material
/// buffer by making requests and accepting materials in an attempt to fill the
/// buffer fully every time step.  Typical usage involves a few things.  In
/// your agent constructor:
///
/// @code
/// class YourAgent : public cyclus::Facility {
///  public:
///   YourAgent(cyclus::Context* ctx) : policy_(this) {...};
///   ...
///   
///   void EnterNotify() {
///     cyclus::Facility::EnterNotify();
///
///     policy_.Init(&inbuf_, "inbuf").Set(incommod);
///     context()->RegisterTrader(&policy_);
///   }
///
///   void Decommission() {
///     context()->UnregisterTrader(&policy_);
///
///     cyclus::Facility::Decommission();
///   }
///   ...
///   private:
///    BuyPolicy policy_;
///    ResourceBuff inbuf_;
///    ...
/// }
///
/// @endcode
///
/// The policy needs to be initialized with its owning agent in the agent's
/// constructor.  It needs to be initialized with the material buffer that is
/// is managing and registered as a Trader in the agent's EnterNotify function.
/// And it also needs to be unregistered as a trader in the agent's
/// Decommission function.
class BuyPolicy : public cyclus::Trader {
 public:
  BuyPolicy(cyclus::Agent* manager) : cyclus::Trader(manager) {};

  virtual ~BuyPolicy() {};

  /// Init configures the policy to keep buf full every time step.  If
  /// quantize is greater than zero, the policy will make exclusive, integral
  /// quantize kg requests.  Otherwise, single requests will be sent to
  /// fill the buffer's empty space.
  BuyPolicy& Init(cyclus::toolkit::ResourceBuff* buf, std::string name, double quantize = -1);

  /// Set configures the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can have an
  /// arbitrary number of policies set.  Recalling Set to modify the composition
  /// or preference of a commodity that has previously been set is allowed.
  BuyPolicy& Set(std::string commod, cyclus::Composition::Ptr c, double pref = 0.0);

  /// Commods returns materials and their respective commodities that were
  /// received on the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  std::map<cyclus::Material::Ptr, std::string> Commods();

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
