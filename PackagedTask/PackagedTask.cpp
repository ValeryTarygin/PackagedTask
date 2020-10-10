#include <iostream>
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>
#include <chrono>

using namespace std;


deque< packaged_task<void()> > tasks;
mutex mut;
bool flagExit = false;

void funcA()
{
    cout << "AAA\n";
}

int funcB()
{
	cout << "BBB\n";
	throw 1;
	return 1;
}

void funcC()
{
	cout << "CCC\n";
}

void proccess()
{
	while (!flagExit)
	{
		packaged_task<void()> pck;
		{
			lock_guard<mutex> lck(mut);
			if (!tasks.empty())
			{
				pck = move(tasks.front());
				tasks.pop_front();
				pck();
			}
		}
		
		this_thread::sleep_for(chrono::seconds(5));
	}
}

template<typename Func>
future<void> putFunc(Func f)
{
	packaged_task<void()> pack(f);
	future<void> fut = pack.get_future();
	lock_guard<mutex> lck(mut);
	tasks.push_back(move(pack));

	return fut;
}


int main()
{
	int val;

	thread t = thread(proccess);
	future<void> f;
	while (true)
	{
		cin >> val;
		if (val == 1)
			f = putFunc(funcA);
		else if (val == 2)
			f = putFunc(funcB);
		else if (val == 3)
		{
			f = putFunc(funcC);
			f = putFunc([]() { cout << "FINISH\n"; });
		}
		else
		{
			flagExit = true;
			break;
		}

		try
		{
			f.get();
		}
		catch (...)
		{
			int a = 9;
		}
	}

	t.join();
}

