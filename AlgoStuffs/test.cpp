#pragma once
#include "imports.h"
#include "acs.cpp"
#include "kmp.h"
#include "bm.h"
//#include "temp2.cpp"
const char TEST_WILDCARD = '?';

bool IsMatch(string &array, int &position, string &pattern) 
{
	if (pattern.size() > (array.size() - position))
		return false;
	for (int i = 0; i < pattern.size(); i++)
		if (array[position + i] != pattern[i] && pattern[i] != TEST_WILDCARD)
			return false;
	return true;
}


void test() {

	std::string TEST_PATTERN[] = {
		"0001400006",
		"505250535400C20200000000000000000000C30200000000000000000000C40200000000000000000000C50200000000000000000000C60200000000000000000000C70200000000000000000000C80200000000000000000000",
		"444E414D9400000000000000803F0000803F000000000000803F0000803F00008043000080430000000000000000000000000000803F0100000000000000000000000001000000000A000040400000803E010000004B000400000030C70300A8C25100000000000000000000000000AF982400AC2615000000000000ABAAEA3F00000000A041000000000000000001000000FFFF7F7F00000000",
		"455459500400AC6A0400",
		"535045447000000000009A19B9420000A54300000000000000009A19B9420000A54300000000000000009A19B9420000A5430000FA43000000009A19B94200004D4300000000DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0FC93F",
		"4B5744410800E84A0F0027433D00",
		"464C545604000000A041"
	};
	std::string TEST_PATTERN_W[] = {
		"000140??06",
		"505250535400C20200000000000000000000C30200??0000000000000000C40200000000000000000000C5020000??00000000000000C60200000000000000000000C70200000000000000000000C?0200000000000000000000",
		"444E414D9?00000000000000803F0000803F000000000000803F0000803F000080430000804300000000??000000000000000000803F010000000000000000000000000100000000??000040400000803E?10000004B000400000030C70300A8C25100000000000000000000000000AF982400AC2615000000000000ABAAEA3F00000000A041000000000000000001000000FFFF7F7F00000000",
		"455459500400AC6A0400",
		"535045447000000000009A19B9420000A5430000000000??00009A19B9420000A54300000000000000009A19B9420000A5430000FA4300??00009A19B94200004D4300000000DB0F4940D?0F4940DB0F4940DB?F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0F4940DB0FC93F",
		"4B57??410800E84A??0027433D00",
		"464C545?0400??00A041"
	};
	std::string TEST_FILE = readfile("C:\\Users\\boring\\source\\repos\\Fallout76-Patcher\\Fallout76-Patcher\\bin\\Release\\net6.0\\win10-x64\\publish\\win10-x64\\SeventySix.esm", true);


	// regex
	// for (int s = 0; s < 6; s++)  for (int i = 0; i < TEST_PATTERN_W[s].size(); i++) if (i + 1 > TEST_PATTERN_W[s].size()) { TEST_PATTERN_W[s].replace(i, 1, "?[0-9a-f]");; } else if (TEST_PATTERN_W[s][i] == '?' && TEST_PATTERN_W[s][i + 1] != '[') TEST_PATTERN_W[s].replace(i, 1, "?[0-9a-f]");

	Timer t;

	/*std::string TEST_FILE = "abbacdabbaaaabbacdaaaaa";
	std::string TEST_PATTERN[] = { "abca" };
	std::string TEST_PATTERN_W[] = { "abb???a" };*/

	/*std::cout << "ACS START\n\n";
	t = Timer();
	t.tick();
	Aho a{};
	for (std::string s : TEST_PATTERN) a.add_string(s);
	a.prepare();
	int b = a.process(TEST_FILE);
	t.tock();
	std::cout << b << std::endl;
	std::cout << std::endl << "acs end in " << t.duration().count() << " seconds" << std::endl;
	*/

	std::cout << "BMH START\n\n";
	t = Timer();
	t.tick();
	size_t res = 0;
	for (std::string s : TEST_PATTERN) res += bmh(TEST_FILE, s).size();
	t.tock();
	std::cout << res << std::endl;
	std::cout << std::endl << "bmh end in " << t.duration().count() << " seconds" << std::endl;
	

	std::cout << "match START\n\n";
	t = Timer();
	t.tick();
	res = 0;
	for (std::string s : TEST_PATTERN) {
		//std::cout << match(s, TEST_FILE) << std::endl;
		int m = s.size();
		int i = 0, index = 0;
		int n = TEST_FILE.size();
		while (index <= (n - m)) {
			for (int i = m - 1; TEST_FILE[index + i] == s[i] || s[i] == TEST_WILDCARD; i--)
			{
				if (i == 0)
				{
					res += 1;
					index += m;
					break;
				}
			}
			index += 1;
		}
	}
	//for (std::string s : TEST_PATTERN_W) res += bmh_w(TEST_FILE, s, TEST_WILDCARD).size();
	t.tock();
	std::cout << res << std::endl;
	std::cout << std::endl << "match end in " << t.duration().count() << " seconds" << std::endl;

	std::cout << "test start\n\n";

	t = Timer();
	t.tick();
	res = 0;

	for (std::string s : TEST_PATTERN) {
		int m = TEST_FILE.size();
		for (int i = 0; i < m; i++) {
			if (!IsMatch(TEST_FILE, i, s)) continue;
			res += 1;
			if (i + s.size() > m) i += s.size();
		}
	}

	t.tock();
	std::cout << res << std::endl;
	std::cout << std::endl << "test end in " << t.duration().count() << " seconds" << std::endl;

	/*std::cout << "KMP START\n\n";
	t = Timer();
	t.tick();
	int res = 0;
	for (std::string s : TEST_PATTERN) res += search(TEST_FILE, s).size();
	t.tock();
	std::cout << std::endl << "kmp end in " << t.duration().count() << " seconds" << std::endl;*/

	/*std::cout << "KMP_W START\n\n";
	t = Timer();
	t.tick();
	res = 0;
	for (std::string s : TEST_PATTERN_W) res += search(TEST_FILE, s, TEST_WILDCARD).size();
	t.tock();
	std::cout << std::endl << "kmp_w end in " << t.duration().count() << " seconds" << std::endl;*/
}

int main() {
	test();

	std::string dummy;
	std::cin >> dummy;
}