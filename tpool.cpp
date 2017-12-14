/*
 * tpool.cpp
 *
 * Description: Thread Pool Example.
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the LICENSE
 * file included in the distribution.
 *
 */

#include "tpool.h"

void myfunc() {
	cout << "Hello!" << endl;
}

int main() {
	ThreadPool pool(3);
	func_t func = bind(myfunc);

	pool.Run(func);
	pool.Run(func);
	pool.Run(func);

	cin.get();

	return 0;
}
