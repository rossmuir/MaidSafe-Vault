/* Copyright 2013 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#include "maidsafe/vault/version_manager/action_get.h"
#include "maidsafe/vault/version_manager/action_get_version.pb.h"


namespace maidsafe {

namespace vault {

const nfs::MessageAction ActionVersionManagerGet::kActionId;

ActionVersionManagerGet::ActionVersionManagerGet() {}

void ActionVersionManagerGet::operator()(
    boost::optional<VersionManagerValue>& value,
    std::vector<StructuredDataVersions::VersionName>& version_names) const {
  version_names.clear();
  if (value)
    version_names = value->Get();
}

}  // namespace vault

}  // namespace maidsafe