/*  Copyright 2012 MaidSafe.net limited

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

#include "maidsafe/vault/tests/vault_network.h"

#include <algorithm>
#include <string>

#include "maidsafe/common/test.h"
#include "maidsafe/vault/tests/tests_utils.h"

namespace maidsafe {

namespace vault {

namespace test {

class VaultNetworkTest : public VaultNetwork  {
 public:
  VaultNetworkTest() {}
};

TEST_F(VaultNetworkTest, FUNC_BasicSetup) {
}

TEST_F(VaultNetworkTest, FUNC_VaultJoins) {
  Sleep(std::chrono::seconds(2));
  LOG(kVerbose) << "Adding a vault";
  EXPECT_TRUE(Add());
}

TEST_F(VaultNetworkTest, FUNC_ClientJoins) {
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(false));
  Sleep(std::chrono::seconds(2));
}

TEST_F(VaultNetworkTest, FUNC_PmidRegisteringClientJoins) {
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
}

TEST_F(VaultNetworkTest, FUNC_MultipleClientsJoin) {
  for (int index(0); index < 5; ++index) {
    Sleep(std::chrono::seconds(2));
    EXPECT_TRUE(AddClient(false));
  }
  Sleep(std::chrono::seconds(2));
}

TEST_F(VaultNetworkTest, FUNC_PutGetDelete) {
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  ImmutableData data(NonEmptyString(RandomString(1024)));
  LOG(kVerbose) << "Before put";
  try {
    EXPECT_NO_THROW(clients_[0]->nfs_->Put(data));
    LOG(kVerbose) << "After put";
  }
  catch (...) {
    EXPECT_TRUE(false) << "Failed to put: " << DebugId(NodeId(data.name()->string()));
  }
  Sleep(std::chrono::seconds(5));
  auto future(clients_[0]->nfs_->Get<ImmutableData::Name>(data.name(), std::chrono::seconds(5)));
  try {
    auto retrieved(future.get());
    EXPECT_EQ(retrieved.data(), data.data());
  }
  catch (...) {
    EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
  }

  clients_[0]->nfs_->Delete<ImmutableData::Name>(data.name());
  Sleep(std::chrono::seconds(5));

  routing::Parameters::caching = false;

  future = clients_[0]->nfs_->Get<ImmutableData::Name>(data.name());
  try {
    EXPECT_THROW(future.get(), std::exception) << "should have failed retreiveing data: "
                                               << DebugId(NodeId(data.name()->string()));
  }
  catch (const std::exception& e) {
    LOG(kVerbose) << DebugId(NodeId(data.name()->string())) << " Deleted "
                  << boost::diagnostic_information(e);
  }
  Sleep(std::chrono::seconds(5));
}

TEST_F(VaultNetworkTest, FUNC_MultiplePuts) {
  Sleep(std::chrono::seconds(2));
  ASSERT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  const size_t kIterations(10);
  std::vector<ImmutableData> chunks;
  for (auto index(kIterations); index > 0; --index)
    chunks.emplace_back(NonEmptyString(RandomString(1024)));

  int index(0);
  for (const auto& chunk : chunks) {
    EXPECT_NO_THROW(clients_[0]->nfs_->Put(chunk)) << "Store failure "
                                                   << DebugId(NodeId(chunk.name()->string()));
    Sleep(std::chrono::seconds(1));
    LOG(kVerbose) << DebugId(NodeId(chunk.name()->string())) << " stored: " << index++;
  }

  std::vector<boost::future<ImmutableData>> get_futures;
  for (const auto& chunk : chunks) {
    get_futures.emplace_back(
        clients_[0]->nfs_->Get<ImmutableData::Name>(chunk.name(),
                                                    std::chrono::seconds(kIterations)));
  }

  for (size_t index(0); index < kIterations; ++index) {
    try {
      auto retrieved(get_futures[index].get());
      EXPECT_EQ(retrieved.data(), chunks[index].data());
      LOG(kVerbose) << "Retrieved: " << index;
    }
    catch (const std::exception& ex) {
      EXPECT_TRUE(false) << "Failed to retrieve chunk: " << DebugId(chunks[index].name())
                         << " because: " << boost::diagnostic_information(ex) << " "  << index;
    }
  }

  Sleep(std::chrono::seconds(5));
  LOG(kVerbose) << "Multiple puts is finished successfully";
}

TEST_F(VaultNetworkTest, FUNC_FailingGet) {
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  LOG(kVerbose) << "Client joins";
  ImmutableData data(NonEmptyString(RandomString(1024)));
  EXPECT_THROW(Get<ImmutableData>(data.name()), std::exception) << "must have failed";
}

TEST_F(VaultNetworkTest, FUNC_PutMultipleCopies) {
  Sleep(std::chrono::seconds(2));
  LOG(kVerbose) << "Clients joining";
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  LOG(kVerbose) << "Clients joined";

  ImmutableData data(NonEmptyString(RandomString(1024)));
  boost::future<ImmutableData> future;
  clients_[0]->nfs_->Put(data);
  Sleep(std::chrono::seconds(2));

  clients_[1]->nfs_->Put(data);
  Sleep(std::chrono::seconds(2));

  {
    future = clients_[0]->nfs_->Get<ImmutableData::Name>(data.name());
    try {
      auto retrieved(future.get());
      EXPECT_EQ(retrieved.data(), data.data());
    }
    catch (...) {
      EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
    }
  }

  LOG(kVerbose) << "1st successful put";

  {
    future = clients_[1]->nfs_->Get<ImmutableData::Name>(data.name());
    try {
      auto retrieved(future.get());
      EXPECT_EQ(retrieved.data(), data.data());
    }
    catch (...) {
      EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
    }
  }

  LOG(kVerbose) << "2nd successful put";

  clients_[0]->nfs_->Delete<ImmutableData::Name>(data.name());
  Sleep(std::chrono::seconds(2));

  LOG(kVerbose) << "1st Delete the chunk";

  try {
    auto retrieved(Get<ImmutableData>(data.name()));
    EXPECT_EQ(retrieved.data(), data.data());
  }
  catch (...) {
    EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
  }

  LOG(kVerbose) << "Chunk still exist as expected";

  clients_[1]->nfs_->Delete<ImmutableData::Name>(data.name());
  Sleep(std::chrono::seconds(2));

  LOG(kVerbose) << "2nd Delete the chunk";

  routing::Parameters::caching = false;

  EXPECT_THROW(Get<ImmutableData>(data.name()), std::exception) << "Should have failed to retreive";
  Sleep(std::chrono::seconds(5));
  LOG(kVerbose) << "PutMultipleCopies Succeeds";
}

TEST_F(VaultNetworkTest, FUNC_MultipleClientsPut) {
  int clients(5);
  for (int index(0); index < clients; ++index) {
    Sleep(std::chrono::seconds(2));
    std::cout << "joining client " << index << std::endl;
    EXPECT_TRUE(AddClient(true));
  }
  Sleep(std::chrono::seconds(2));
  LOG(kVerbose) << "Clients joined...";
  const size_t kIterations(10);
  std::vector<ImmutableData> chunks;
  for (auto index(kIterations); index > 0; --index)
    chunks.emplace_back(NonEmptyString(RandomString(1024)));

  for (const auto& chunk : chunks) {
    LOG(kVerbose) << "Storing: " << DebugId(chunk.name());
    EXPECT_NO_THROW(clients_[RandomInt32() % clients]->nfs_->Put(chunk));
    Sleep(std::chrono::seconds(2));
  }

  LOG(kVerbose) << "Chunks are sent to be stored...";

  std::vector<boost::future<ImmutableData>> get_futures;
  for (const auto& chunk : chunks) {
    LOG(kVerbose) << "geting chunk : " << DebugId(chunk.name());
    get_futures.emplace_back(
        clients_[RandomInt32() % clients]->nfs_->Get<ImmutableData::Name>(chunk.name()));
    Sleep(std::chrono::seconds(2));
  }

  for (size_t index(0); index < kIterations; ++index) {
    try {
      auto retrieved(get_futures[index].get());
      EXPECT_EQ(retrieved.data(), chunks[index].data());
    }
    catch (const std::exception& ex) {
      LOG(kError) << "Failed to retrieve chunk: " << DebugId(chunks[index].name())
                  << " because: " << boost::diagnostic_information(ex);
    }
  }
  Sleep(std::chrono::seconds(5));
  for (auto& client : clients_)
    client.reset();
  Sleep(std::chrono::seconds(1));
  clients_.clear();
  LOG(kInfo) << "clients destructed";
  Sleep(std::chrono::seconds(5));
}

TEST_F(VaultNetworkTest, FUNC_UnauthorisedDelete) {
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));
  EXPECT_TRUE(AddClient(true));
  Sleep(std::chrono::seconds(2));

  routing::Parameters::caching = false;
  ImmutableData chunk(NonEmptyString(RandomString(2^10)));
  EXPECT_NO_THROW(clients_.front()->nfs_->Put<ImmutableData>(chunk)) << "should have succeeded";
  Sleep(std::chrono::seconds(3));
  EXPECT_NO_THROW(Get<ImmutableData>(chunk.name()));
  LOG(kVerbose) << "Chunk is verified to be in the network";
  clients_.back()->nfs_->Delete(chunk.name());
  Sleep(std::chrono::seconds(3));
  EXPECT_NO_THROW(Get<ImmutableData>(chunk.name())) << "Delete must have failed";
  Sleep(std::chrono::seconds(3));
  clients_.front()->nfs_->Delete(chunk.name());
  Sleep(std::chrono::seconds(3));
  EXPECT_THROW(Get<ImmutableData>(chunk.name()), std::exception)  << "Delete must have succeeded";
  Sleep(std::chrono::seconds(5));
}

}  // namespace test

}  // namespace vault

}  // namespace maidsafe

