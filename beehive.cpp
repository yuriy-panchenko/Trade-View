#include "pch.h"
#include "beehive.h"

std::wstring GetLastErrorAsString();

CHive::CHive()
	:maxNum{ std::thread::hardware_concurrency() }
{
}

CHive::~CHive()
{
	while (Wait());
}

CWinThread* CHive::Add(CWinThread* pTh)
{
	CWinThread* ret{ nullptr };
	if (ths.size() >= maxNum)
		ret = Wait();
	ths.push_back(pTh);
	return ret;
}

CWinThread* CHive::Wait()
{
	std::vector<HANDLE> hs;
	hs.reserve(ths.size());
	auto iter{ hs.begin() };
	CWinThread* ret{ nullptr };

	for (auto iter{ ths.begin() }; iter != ths.end(); iter++)
		if (**iter == INVALID_HANDLE_VALUE)
		{
			ret = *iter;
			ths.erase(iter);
			return ret;
		}
		else hs.push_back(**iter);

	if (!hs.empty())
	{
		auto const res{ ::WaitForMultipleObjects((DWORD)hs.size(), hs.data(), FALSE, INFINITE) };

		switch (res)
		{
		case WAIT_FAILED:
		{
			auto text = GetLastErrorAsString();
			OutputDebugString(text.c_str());
		}
		case WAIT_TIMEOUT:
			return nullptr;
		default:
		{
			auto iter{ ths.begin() + (res - WAIT_OBJECT_0) };
			ret = *iter;
			ths.erase(iter);
		}
		}
	}

	return ret;
}

std::wstring GetLastErrorAsString()
{
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return L"No error"; // No error message has been recorded

	LPWSTR messageBuffer = nullptr;

	// Get the error message, if any.
	size_t size = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);

	// Free the buffer allocated by FormatMessage
	LocalFree(messageBuffer);

	return message;
}
