#pragma once
template<typename Key = std::wstring, typename Value = Key>
class IniFile
{
	using record = std::pair<Key, Value>;
	using order_list = std::list<Key>;
	using data_map = std::unordered_map<Key, Value>;

	order_list m_Order;
	data_map m_Data;

public:
	IniFile() = default;
	IniFile(CString const&);

	void insert(record&&);
	order_list const& keys()const { return m_Order; }
	Value const& operator[](Key const&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Key, typename Value>
IniFile<Key,Value>::IniFile(CString const& text)
{
	auto split = [](CString const& str)->record
		{
			auto const eqIndex{ str.Find(_T('=')) };
			std::wstring key{ str.Left(eqIndex) }, value{ str.Right(str.GetLength() - eqIndex - 1) };
			auto dotIndex{ value.find(L".") };
			if (dotIndex != Value::npos)
			{
				auto riter{ value.rbegin() };
				for (; riter != value.rend(); ++riter)
					if (*riter == L'.')
					{
						++riter;
						break;
					}
					else if (*riter != L'0')
						break;

				value.erase(riter.base(), value.end());
			}

			return { key, value };
		};


	if (!text.IsEmpty())
	{
		int pos{ 0 };
		auto str{ text.Tokenize(_T("\r\n"), pos) };
		while (!str.IsEmpty())
		{
			insert(split(str));
			str = text.Tokenize(_T("\r\n"), pos);
		}
	}
}

template<typename Key, typename Value>
void IniFile<Key, Value>::insert(record&& rec)
{
	if (m_Data.contains(rec.first))
		m_Data[rec.first] = std::move(rec.second);
	else
	{
		m_Order.push_back(rec.first);
		m_Data.insert(std::move(rec));
	}
}

template<typename Key, typename Value>
Value const& IniFile<Key, Value>::operator[](Key const& key)
{
	return m_Data[key];
}
