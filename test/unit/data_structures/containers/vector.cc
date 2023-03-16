/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "basic_test.h"
#include "test_init.h"
#include "hermes_shm/data_structures/thread_unsafe/vector.h"
#include "hermes_shm/data_structures/thread_unsafe/list.h"
#include "hermes_shm/data_structures/string.h"
#include "vector.h"

using hermes_shm::ipc::vector;
using hermes_shm::ipc::list;
using hermes_shm::ipc::string;

template<typename T>
void VectorTestRunner(VectorTestSuite<T, vector<T>> &test) {
  test.EmplaceTest(15);
  test.IndexTest();
  test.ForwardIteratorTest();
  test.ConstForwardIteratorTest();
  test.CopyConstructorTest();
  test.CopyAssignmentTest();
  test.MoveConstructorTest();
  test.MoveAssignmentTest();
  test.EmplaceFrontTest();
  test.ModifyEntryCopyIntoTest();
  test.ModifyEntryMoveIntoTest();
  test.EraseTest();
}

template<typename T, bool ptr>
void VectorTest() {
  Allocator *alloc = alloc_g;
  if constexpr(ptr) {
    auto vec = hipc::make_uptr<vector<T>>(alloc);
    VectorTestSuite<T, vector<T>> test(*vec, alloc);
    VectorTestRunner<T>(test);
  } else {
    vector<T> vec(alloc);
    VectorTestSuite<T, vector<T>> test(vec, alloc);
    VectorTestRunner<T>(test);
  }
}

void VectorOfVectorOfStringTest() {
  Allocator *alloc = alloc_g;
  vector<vector<string>> vec(alloc);

  vec.resize(10);
  for (hipc::Ref<vector<string>> bkt : vec) {
    bkt->emplace_back("hello");
  }
  vec.clear();/**/
}

void VectorOfListOfStringTest() {
  Allocator *alloc = alloc_g;
  vector<list<string>> vec(alloc);

  vec.resize(10);
  for (hipc::Ref<list<string>> bkt : vec) {
    bkt->emplace_back("hello");
  }
  vec.clear();
}

TEST_CASE("VectorOfInt") {
  Allocator *alloc = alloc_g;
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
  VectorTest<int, false>();
  VectorTest<int, true>();
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
}

TEST_CASE("VectorOfString") {
  Allocator *alloc = alloc_g;
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
  VectorTest<hipc::string, false>();
  VectorTest<int, true>();
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
}

TEST_CASE("VectorOfStdString") {
  Allocator *alloc = alloc_g;
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
  VectorTest<std::string, false>();
  VectorTest<int, true>();
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
}

TEST_CASE("VectorOfVectorOfString") {
  Allocator *alloc = alloc_g;
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
  VectorOfVectorOfStringTest();
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
}

TEST_CASE("VectorOfListOfString") {
  Allocator *alloc = alloc_g;
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
  VectorOfListOfStringTest();
  REQUIRE(alloc->GetCurrentlyAllocatedSize() == 0);
}
