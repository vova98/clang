#include <iostream>
#include <vector>

int main(int argc, char* argv[]){

	std::vector<int> vec = {0,2,3,4,6,7,1};

	for (std::vector<int>::iterator s = vec.begin(); s != vec.end(); ++s){
		std::cout << *s << '\n';
	}

	return 0;
}
