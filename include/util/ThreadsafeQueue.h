/*
 * ThreadsafeQueue.h
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#ifndef THREADSAFEQUEUE_H_
#define THREADSAFEQUEUE_H_

#include <memory>
#include <optional>
#include <condition_variable>
#include <queue>
#include <thread>
#include <mutex>

namespace othello {

template<typename T>
class ts_queue final {
public:
	ts_queue() = default;
	~ts_queue() = default;
	ts_queue(const ts_queue &other) = default;
	ts_queue(ts_queue &&other) = default;
	ts_queue& operator=(const ts_queue &other) = default;
	ts_queue& operator=(ts_queue &&other) = default;

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.size();
	}

	/**
	 * Blocks until the underlying queue is not empty or ms milliseconds has been elapsed
	 * and then return the front element
	 * @param ms
	 * @return
	 */
	std::optional<std::shared_ptr<T>> wait_and_pop(unsigned short ms) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait_for(lock, std::chrono::milliseconds(ms),
				[this]() { return !m_queue.empty(); });

		if (!m_queue.empty()) {
			std::shared_ptr<T> value = m_queue.front();//std::move(m_queue.front());
			m_queue.pop();
			return value;
		}

		return std::nullopt;
	}


//	/**
//	 * Blocks until the underlying queue is not empty or 500 ms has been elapsed
//	 * and then return the front element
//	 * @param value the element to
//	 * @return
//	 */
//	std::optional<std::shared_ptr<T>> wait_and_pop() {
//		std::unique_lock<std::mutex> lock(m_mutex);
//		m_cv.wait_for(lock, std::chrono::milliseconds(500),
//				[this]() { return !m_queue.empty(); });
//
//		if (!m_queue.empty()) {
//			std::shared_ptr<T> value = m_queue.front();//std::move(m_queue.front());
//			m_queue.pop();
//			return value;
//		}
//
//		return std::nullopt;
//	}

	//	T& front() {
	//		std::lock_guard<std::mutex> lock(m_mutex);
	//		return m_queue.front();
	//	}
	//
	//	T& back() {
	//		std::lock_guard<std::mutex> lock(m_mutex);
	//		return m_queue.back();
	//	}
	//
	//	void pop() {
	//		std::lock_guard<std::mutex> lock(m_mutex);
	//		m_queue.pop();
	//	}

	void push(std::shared_ptr<T>& node) {
		//std::cout << "\nTrying to acquire lock for pushing..." << std::flush;
		std::lock_guard<std::mutex> lock(m_mutex);
		//std::cout << "\nPushing node:" << *(pNode.get()) << std::flush;
		m_queue.push(node);
		//std::cout << "\nNotifying pushing node:" << *(pNode.get()) << std::flush;
		m_cv.notify_one();
	}

	void push(std::shared_ptr<T>&& pNode) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(pNode);
		m_cv.notify_one();
	}

private:
	std::queue<std::shared_ptr<T>> m_queue;
	std::condition_variable m_cv;
	mutable std::mutex m_mutex;
};


} /* namespace othello */

#endif /* THREADSAFEQUEUE_H_ */
