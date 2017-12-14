/*
 * tpool.h
 *
 * Description: Thread Pool Class (Example).
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the LICENSE
 * file included in the distribution.
 *
 */

#ifndef _TPOOL_H
#define _TPOOL_H

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>

#if __cplusplus <= 199711L
	#error This program needs at least a C++11 compliant compiler
#endif

using namespace std;
typedef function<void()> func_t;

struct Worker {
	int id;
	bool working;
	thread worker;

	Worker(int id) : id(id), working(false) {}
};

class ThreadPool {
public:
	ThreadPool(int nmt) {
		numThreads = nmt;
		for (int i = 0; i < numThreads; i++)
			threads.push_back(Worker(i));
		tasks_scheduler = thread(&ThreadPool::scheduler, this);
		tasks_scheduler.detach();
		stop = false;
	}

	~ThreadPool() {
		stop = true;
	}

	void Run(func_t func) {
		cout << "add task" << endl;
		qmutex.lock();
		tasks.push(func);
		qmutex.unlock();
	}

private:
	int numThreads;
	vector<Worker> threads;
	thread tasks_scheduler;
	queue<func_t> tasks;
	mutex qmutex;
	mutex tmutex;
	bool stop;

	void scheduler() {
		func_t task = NULL;
		int thread_id = -1;

		cout << "run sheduler" << endl;
		while (!stop) {
			if (task == NULL)
				task = getTask();

			if (thread_id == -1)
				thread_id = getThread();

			if ((thread_id != -1) && (task != NULL)) {
				threads[thread_id].worker = thread(&ThreadPool::taskWrapper, this, thread_id, task);
				threads[thread_id].worker.detach();

				task = NULL;
				thread_id = -1;
			}
		}
	}

	void taskWrapper(int thread_id, func_t task) {
		cout << "executing" << endl;
		task();
		tmutex.lock();
		threads[thread_id].working = false;
		tmutex.unlock();
	}

	int getThread() {
		int thread_id = -1;

		tmutex.lock();
		for (int i = 0; i < numThreads; i++) {
			if (!threads[i].working) {
				threads[i].working = true;
				thread_id = i;
				break;
			}
		}
		tmutex.unlock();
		return thread_id;
	}

	func_t getTask() {
		func_t func = NULL;
		qmutex.lock();
		if (!tasks.empty()) {
			func = tasks.front();
			tasks.pop();
		}
		qmutex.unlock();

		return func;
	}
};

#endif // _TPOOL_H
