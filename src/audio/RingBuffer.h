#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <atomic>
#include <vector>
#include <cstring>
#include <algorithm>

/**
 * @brief Lock-free ring buffer for audio streaming
 * 
 * Single-producer, single-consumer (SPSC) ring buffer optimized for audio.
 * Thread-safe without locks using atomic operations.
 * 
 * Usage:
 * - Main thread: Writes audio samples with Write()
 * - Audio callback thread: Reads samples with Read()
 */
template<typename T>
class RingBuffer {
public:
    /**
     * @brief Construct ring buffer
     * @param capacity Number of elements (should be power of 2 for best performance)
     */
    explicit RingBuffer(size_t capacity)
        : buffer_(capacity)
        , capacity_(capacity)
        , writePos_(0)
        , readPos_(0) {
    }

    /**
     * @brief Write data to ring buffer
     * @param data Pointer to data
     * @param count Number of elements to write
     * @return Number of elements actually written
     */
    size_t Write(const T* data, size_t count) {
        const size_t available = GetWriteAvailable();
        const size_t toWrite = std::min(count, available);
        
        if (toWrite == 0) {
            return 0;
        }

        const size_t writeIdx = writePos_.load(std::memory_order_relaxed);
        const size_t firstChunk = std::min(toWrite, capacity_ - writeIdx);
        
        // Write first chunk (up to end of buffer)
        std::memcpy(&buffer_[writeIdx], data, firstChunk * sizeof(T));
        
        // Write second chunk (wrapped around to beginning)
        if (toWrite > firstChunk) {
            std::memcpy(&buffer_[0], data + firstChunk, (toWrite - firstChunk) * sizeof(T));
        }
        
        // Update write position with release semantics (ensures writes are visible)
        writePos_.store((writeIdx + toWrite) % capacity_, std::memory_order_release);
        
        return toWrite;
    }

    /**
     * @brief Read data from ring buffer
     * @param data Pointer to destination buffer
     * @param count Number of elements to read
     * @return Number of elements actually read
     */
    size_t Read(T* data, size_t count) {
        const size_t available = GetReadAvailable();
        const size_t toRead = std::min(count, available);
        
        if (toRead == 0) {
            // No data available - fill with silence
            std::memset(data, 0, count * sizeof(T));
            return 0;
        }

        const size_t readIdx = readPos_.load(std::memory_order_relaxed);
        const size_t firstChunk = std::min(toRead, capacity_ - readIdx);
        
        // Read first chunk
        std::memcpy(data, &buffer_[readIdx], firstChunk * sizeof(T));
        
        // Read second chunk (wrapped)
        if (toRead > firstChunk) {
            std::memcpy(data + firstChunk, &buffer_[0], (toRead - firstChunk) * sizeof(T));
        }
        
        // Fill remaining with silence if we read less than requested
        if (toRead < count) {
            std::memset(data + toRead, 0, (count - toRead) * sizeof(T));
        }
        
        // Update read position with release semantics
        readPos_.store((readIdx + toRead) % capacity_, std::memory_order_release);
        
        return toRead;
    }

    /**
     * @brief Get number of elements available to write
     */
    size_t GetWriteAvailable() const {
        const size_t writeIdx = writePos_.load(std::memory_order_relaxed);
        const size_t readIdx = readPos_.load(std::memory_order_acquire);
        
        if (writeIdx >= readIdx) {
            return capacity_ - (writeIdx - readIdx) - 1;
        } else {
            return readIdx - writeIdx - 1;
        }
    }

    /**
     * @brief Get number of elements available to read
     */
    size_t GetReadAvailable() const {
        const size_t writeIdx = writePos_.load(std::memory_order_acquire);
        const size_t readIdx = readPos_.load(std::memory_order_relaxed);
        
        if (writeIdx >= readIdx) {
            return writeIdx - readIdx;
        } else {
            return capacity_ - (readIdx - writeIdx);
        }
    }

    /**
     * @brief Clear the buffer
     */
    void Clear() {
        readPos_.store(0, std::memory_order_relaxed);
        writePos_.store(0, std::memory_order_relaxed);
    }

    /**
     * @brief Get buffer capacity
     */
    size_t GetCapacity() const {
        return capacity_;
    }

private:
    std::vector<T> buffer_;
    size_t capacity_;
    std::atomic<size_t> writePos_;
    std::atomic<size_t> readPos_;
};

#endif // RINGBUFFER_H
