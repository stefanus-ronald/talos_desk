// This file has been moved to src/core/frame_buffer.cpp
// and replaced with a high-performance implementation.
// 
// The new FrameBuffer class provides:
// - Circular buffering with lock-free operations
// - Memory pooling for frame reuse
// - Zero-copy optimizations
// - Performance statistics
// 
// Include "core/frame_buffer.h" to use the new implementation.