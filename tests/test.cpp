#include <iostream>
#include <vector>

std::vector<int> container {1, 2, 3, 4, 5};
int main(){
	for (auto i_1 = container.begin(), e = container.end(); i_1 != e; ++i_1) {
		*i_1 += 1;
		std::cout<<*i_1<<"\n";
	}
	for (auto i_1 = container.begin(); i_1 != container.end(); ++i_1) {
		*i_1 += 1;
		std::cout<<*i_1<<"\n";
	}
	for (int k = 0; k < 10; k++)
		std::cout<<k;

	for (auto i_2 : container) {
		i_2 += 1;
		//std::cout<<i_2<<"\n";
	}
	return 0;
}
