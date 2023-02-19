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


#ifndef HERMES_MEMORY_ALLOCATOR_PAGE_ALLOCATOR_H_
#define HERMES_MEMORY_ALLOCATOR_PAGE_ALLOCATOR_H_

#include "allocator.h"
#include "hermes_shm/thread/lock.h"
// #include "hermes_shm/data_structures/thread_unsafe/vector.h"
#include "stack_allocator.h"

namespace hermes::ipc {

struct Page {};

template<typename T>
class vector;

struct PageAllocatorHeader : public AllocatorHeader {
  size_t page_size_;
  std::atomic<size_t> total_alloced_;
  Pointer custom_header_ptr_;

  PageAllocatorHeader() = default;

  void Configure(allocator_id_t alloc_id,
                 size_t custom_header_size,
                 size_t page_size) {
    AllocatorHeader::Configure(alloc_id, AllocatorType::kPageAllocator,
                               custom_header_size);
    page_size_ = page_size;
  }
};

class PageAllocator : public Allocator {
 private:
  PageAllocatorHeader *header_;
  ArrayBackend ibackend_;
  StackAllocator ialloc_;

 public:
  /**
   * Allocator constructor
   * */
  PageAllocator()
  : header_(nullptr) {}

  /**
   * Get the ID of this allocator from shared memory
   * */
  allocator_id_t GetId() override {
    return header_->allocator_id_;
  }

  /**
   * Initialize the allocator in shared memory
   * */
  void shm_init(MemoryBackend *backend,
                allocator_id_t id,
                size_t custom_header_size = 0,
                size_t page_size = KILOBYTES(4));

  /**
   * Attach an existing allocator from shared memory
   * */
  void shm_deserialize(MemoryBackend *backend) override;

  /**
   * Allocate a memory of \a size size. The page allocator cannot allocate
   * memory larger than the page size.
   * */
  OffsetPointer AllocateOffset(size_t size) override;

  /**
   * Allocate a memory of \a size size, which is aligned to \a
   * alignment.
   * */
  OffsetPointer AlignedAllocateOffset(size_t size, size_t alignment) override;

  /**
   * Reallocate \a p pointer to \a new_size new size.
   *
   * @return whether or not the pointer p was changed
   * */
  OffsetPointer ReallocateOffsetNoNullCheck(
    OffsetPointer p, size_t new_size) override;

  /**
   * Free \a ptr pointer. Null check is performed elsewhere.
   * */
  void FreeOffsetNoNullCheck(OffsetPointer p) override;

  /**
   * Get the current amount of data allocated. Can be used for leak
   * checking.
   * */
  size_t GetCurrentlyAllocatedSize() override;
};

}  // namespace hermes::ipc

#endif  // HERMES_MEMORY_ALLOCATOR_PAGE_ALLOCATOR_H_
