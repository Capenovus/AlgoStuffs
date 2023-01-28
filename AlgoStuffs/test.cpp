#include "acs.h"
#include "kmp.h"
#include "bm.h"
#include <chrono>

int main() {
	const char wildcard = '?';
	std::string pattern = "00 01 40 00 06";
	std::string file = "C:\\Users\\boring\\source\\repos\\Fallout76-Patcher\\Fallout76-Patcher\\bin\\Release\\net6.0\\win10-x64\\publish\\win10-x64\\SeventySix.esm";

	pattern.erase(std::remove_if(pattern.begin(), pattern.end(), isspace), pattern.end());
	
	
	Timer clock;
	std::vector<size_t> t;
	/* SLOOOOOOW
	
	Timer clock;
	clock.tick();
	std::vector<size_t> t = AcsFindFuzzyHexMatchesFromFile(pattern, "C:\\Users\\boring\\source\\repos\\Fallout76-Patcher\\Fallout76-Patcher\\bin\\Release\\net6.0\\win10-x64\\publish\\win10-x64\\SeventySix.esm", wildcard);
	clock.tock();
	std::cout << std::endl << "acs end in " << clock.duration().count() << " seconds" << std::endl;

	for (size_t n : t) {
		std::cout << n << std::endl;
	}*/

	clock = Timer();
	clock.tick();
	t = search(readfile(file, true), pattern);
	clock.tock();
	std::cout << std::endl << "search end in " << clock.duration().count() << " seconds" << std::endl;
	for (size_t n : t) {
		std::cout << n << std::endl;
	}

	std::string pattern2 = "00 01 40 0? 06";

	clock = Timer();
	clock.tick();
	t = search(readfile(file, true), pattern, '?');
	clock.tock();
	std::cout << std::endl << "3 double wildcards search end in " << clock.duration().count() << " seconds" << std::endl;
	for (size_t n : t) {
		std::cout << n << std::endl;
	}

	clock = Timer();
	clock.tick();
	t = fastsearch(readfile(file, true), pattern);
	clock.tock();
	std::cout << std::endl << "fastsearch end in " << clock.duration().count() << " seconds" << std::endl;
	for (size_t n : t) {
		std::cout << n << std::endl;
	}


	std::string dummy;
	std::cin >> dummy;
}