#ifndef KITLUS_SELL_POLICY_H_
#define KITLUS_SELL_POLICY_H_

#include <string>
#include "cyclus.h"

namespace kitlus {

/// SellPolicy performs semi-automatic inventory management of a material
/// buffer by making offers and trading away materials in an attempt to empty
/// the buffer's inventory every time step.
/// For simple behavior, policies virtually eliminate the need to write any code
/// for resource exchange. Just assign a few policies to work with a few
/// buffers and focus on writing the physics and other behvavior of your agent.
/// Typical usage goes something like this:
///
/// @code
/// class YourAgent : public cyclus::Facility {
///  public:
///   ...
///
///   void EnterNotify() {
///     cyclus::Facility::EnterNotify(); // always do this first
///
///     policy_.Init(this, &outbuf_, "outbuf-label").Set(outcommod).Start();
///   }
///   ...
///
///  private:
///   SellPolicy policy_;
///   ResourceBuff outbuf_;
///    ...
/// }
/// @endcode
///
/// The policy needs to be initialized with its owning agent and the material
/// buffer that is is managing. It also needs to be activated by calling the
/// Start function for it to begin participation in resource exchange.  And
/// don't forget to add some commodities to offer on by calling Set.  All
/// policy configuration should usually occur in the agent's EnterNotify member
/// function.
///
/// @warn When a policy's managing agent is deallocated, you MUST either
/// call the policy's Stop function or delete the policy. Otherwise SEGFAULT.
class SellPolicy : public cyclus::Trader {
 public:
  /// Creates an uninitialized policy.  The Init function MUST be called before
  /// anything else is done with the policy.
  SellPolicy();

  virtual ~SellPolicy();

  /// Configures the policy to keep buf empty every time step by offering away
  /// as much material as possible.  If quantize is greater than zero, the
  /// policy will make exclusive, integral quantize kg offers.  Otherwise, a
  /// single offer will be sent each time step to empty the buffer's entire
  /// inventory.
  SellPolicy& Init(cyclus::Agent* manager, cyclus::toolkit::ResourceBuff* buf,
                   std::string name, double quantize = -1);

  /// Instructs the policy to empty its buffer with offers on the given
  /// commodity.  This must be called at least once or the policy will do
  /// nothing.  The policy can offer on an arbitrary number of commodities by
  /// calling Set multiple times.
  SellPolicy& Set(std::string commod);

  /// Registers this policy as a trader in the current simulation.  This
  /// function must be called for the policy to actually participate in resource
  /// exchange. Init MUST be called prior to calling this function.  This is
  /// idempotent.
  void Start();

  /// Unregisters this policy as a trader in the current simulation. This is
  /// idempotent.
  void Stop();

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
