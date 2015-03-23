#if defined(_MSC_VER) && (_MSC_VER <= 1500) //VS2008
	#include <hash_map>
	typedef stdext::hash_map<CHash, int> CHashMapInt;
	typedef stdext::hash_map<int, int> CIntMapInt;
	typedef stdext::hash_map<CHash, std::string> CHashMapString;
#else
	#include <unordered_map>
	typedef std::unordered_map<CHash, int> CHashMapInt;
	typedef std::unordered_map<int, int> CIntMapInt;
	typedef std::unordered_map<CHash, std::string> CHashMapString;
#endif
