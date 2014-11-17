#ifndef KITLUS_BUY_POLICY_H_
#define KITLUS_BUY_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

/// BuyPolicy performs semi-automatic inventory management of a material
/// buffer by making requests and accepting materials in an attempt to fill the
/// buffer fully every time step.  Typical usage goes something like this:
///
/// @code
/// class YourAgent : public cyclus::Facility {
///  public:
///   ...
///
///   void EnterNotify() {
///     cyclus::Facility::EnterNotify(); // always do this first
///
///     policy_.Init(this, &inbuf_, "inbuf-label").Set(incommod, comp).Start();
///   }
///   ...
///
///   private:
///    BuyPolicy policy_;
///    ResourceBuff inbuf_;
///    ...
/// }
/// @endcode
///
/// The policy needs to be initialized with its owning agent and the material
/// buffer that is is managing. It also needs to be activated by calling the
/// Start function for it to begin participation in resource exchange.
/// And don't forget to add some commodities to request by calling Set.  All
/// policy configuration should usually occur in the agent's EnterNotify
/// member function.
class BuyPolicy : public cyclus::Trader {
 public:
  /// Creates an uninitialized policy.  The Init function MUST be called before
  /// anything else is done with the policy.
  BuyPolicy();

  virtual ~BuyPolicy();

  /// Configures the policy to keep buf full every time step.  If
  /// quantize is greater than zero, the policy will make exclusive, integral
  /// quantize kg requests.  Otherwise, single requests will be sent to
  /// fill the buffer's empty space.
  BuyPolicy& Init(cyclus::Agent* manager, cyclus::toolkit::ResourceBuff* buf,
                  std::string name, double quantize = -1);

  /// Instructs the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can
  /// request on an arbitrary number of commodities by calling Set multiple
  /// times.  Re-calling Set to modify the composition
  /// or preference of a commodity that has been set previously is allowed.
  BuyPolicy& Set(std::string commod, cyclus::Composition::Ptr c,
                 double pref = 0.0);

  /// Registers this policy as a trader in the current simulation.  This
  /// function must be called for the policy to actually participate in resource
  /// exchange. Init MUST be called prior to calling this function.  This is
  /// idempotent.
  void Start();

  /// Unregisters this policy as a trader in the current simulation. This is
  /// idempotent.
  void Stop();

  /// Returns corresponding commodities from which each material object
  /// was received for the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  std::map<cyclus::Material::Ptr, std::string> Commods();

  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

  virtual void AcceptMatlTrades(const std::vector<std::pair<
      cyclus::Trade<cyclus::Material>, cyclus::Material::Ptr> >& resps);

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

}  // namespace kitlus

#endif
