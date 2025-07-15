#pragma once

//template<typename Result>
//class bee
//{
//	CWinThread* pThread;
//public:
//	bee() { pThread = ::AfxBeginThread(thProc, this); }
//
//private:
//	static UINT thProc(LPVOID) { return 0; }
//};
//
//template<typename Result, typename ...Args>
//class hive
//{
//	const unsigned int thMax;
//	Result(*m_Proc)(Args...);
//	std::vector<bee<Result>> bees;
//
//public:
//	hive(Result(*proc)(Args...))
//		:m_Proc{ proc }
//		, thMax{ std::thread::hardware_concurrency() }
//	{
//
//	}
//
//	void start(Args...arg)
//	{
//
//		if (bees.size() >= thMax)
//		{
//			//	wait for anything to finish
//		}
//
//		//	add another bee
//		bees.emplace_back();
//	}
//
//private:
//};


class CHive
{
	unsigned int maxNum;
	std::vector<CWinThread*> ths;

public:
	CHive();
	~CHive();
	CWinThread* Add(CWinThread*);
	CWinThread* Wait();
};