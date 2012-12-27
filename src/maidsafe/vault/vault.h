/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#ifndef MAIDSAFE_VAULT_VAULT_H_
#define MAIDSAFE_VAULT_VAULT_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "boost/filesystem/path.hpp"

#include "maidsafe/common/asio_service.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/api_config.h"

#include "maidsafe/vault/public_key_getter.h"
#include "maidsafe/vault/data_holder.h"
#include "maidsafe/vault/maid_account_holder.h"
#include "maidsafe/vault/meta_data_manager.h"
#include "maidsafe/vault/pmid_account_holder.h"
#include "maidsafe/vault/demultiplexer.h"

namespace maidsafe {
namespace routing { class Routing; }  // namespace routing
namespace vault {

class Vault {
 public:
  Vault(passport::Pmid pmid,
        boost::filesystem::path vault_root_dir,
        std::function<void(boost::asio::ip::udp::endpoint)> on_new_bootstrap_endpoint,
        const std::vector<boost::asio::ip::udp::endpoint>& peer_endpoints =
            std::vector<boost::asio::ip::udp::endpoint>());
  ~Vault();  // must issue StopSending() to all identity objects (MM etc.)
            // Then ensure routing is destroyed next then allothers in ny order at this time
 private:
  int InitRouting(const std::vector<boost::asio::ip::udp::endpoint>& peer_endpoints);
  routing::Functors InitialiseRoutingCallbacks();
  void OnMessageReceived(const std::string& message,  const routing::ReplyFunctor& reply_functor);
  void DoOnMessageReceived(const std::string& message, const routing::ReplyFunctor& reply_functor);
  void OnNetworkStatusChange(const int& network_health);
  void DoOnNetworkStatusChange(const int& network_health);
  void OnPublicKeyRequested(const NodeId &node_id, const routing::GivePublicKeyFunctor &give_key);
  void DoOnPublicKeyRequested(const NodeId &node_id, const routing::GivePublicKeyFunctor &give_key);
  void OnCloseNodeReplaced(const std::vector<routing::NodeInfo>& new_close_nodes);
  void DoOnCloseNodeReplaced(const std::vector<routing::NodeInfo>& new_close_nodes);
  void OnStoreCacheData(const std::string& message);
  void DoOnStoreCacheData(const std::string& message);
  bool HaveCacheData(std::string& message);
  void OnNewBootstrapEndpoint(const boost::asio::ip::udp::endpoint& endpoint);
  void DoOnNewBootstrapEndpoint(const boost::asio::ip::udp::endpoint& endpoint);

  std::mutex network_status_mutex_;
  int network_health_;
  std::function<void(boost::asio::ip::udp::endpoint)> on_new_bootstrap_endpoint_;
  std::unique_ptr<routing::Routing> routing_;
  PublicKeyGetter public_key_getter_;
  MaidAccountHolder maid_account_holder_;
  MetadataManager meta_data_manager_;
  PmidAccountHolder pmid_account_holder_;
  DataHolder data_holder_;
  Demultiplexer demux_;
  AsioService asio_service_;
};

}  // namespace vault

}  // namespace maidsafe

#endif  // MAIDSAFE_VAULT_VAULT_H_
