/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_VAULT_CACHE_HANDLER_DISPATCHER_H_
#define MAIDSAFE_VAULT_CACHE_HANDLER_DISPATCHER_H_

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/message_types.h"

#include "maidsafe/vault/messages.h"
#include "maidsafe/vault/message_types.h"
#include "maidsafe/vault/types.h"

namespace maidsafe {

namespace vault {

class CacheHandlerDispatcher {
 public:
  CacheHandlerDispatcher(routing::Routing& routing);
  
  template <typename Data, typename Receiver>
  void SendGetResponse(const Data& data, const Receiver& receiver);

 private:
  CacheHandlerDispatcher();
  CacheHandlerDispatcher(const CacheHandlerDispatcher&);
  CacheHandlerDispatcher(CacheHandlerDispatcher&&);
  CacheHandlerDispatcher& operator=(CacheHandlerDispatcher);

  routing::Routing& routing_;
};

template <typename Data, typename Receiver>
void CacheHandlerDispatcher::SendGetResponse(const Data& /*data*/, const Receiver& /*receiver*/) {}


}  // namespace vault

}  // namespace maidsafe

#endif  // MAIDSAFE_VAULT_CACHE_HANDLER_DISPATCHER_H_
