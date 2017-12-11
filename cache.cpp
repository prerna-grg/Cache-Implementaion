#include <iostream>
#include <map>
#include <fstream>
#include "cache.hpp"
using namespace std;

int main(){
	
	Cache C;
	ifstream file;
	file.open("i.txt");
	char C1;
	int addr;
	while( file >> C1 >> hex >> addr ){
		if( C1=='R' ){
			int y = C.Cread(addr);
		}else{
			C.Cwrite(addr);
		}
	}
	
	cout << "Misses = " << C.getMisses() << endl;
	cout << "Hits = " << C.getHits() << endl;
	cout << "WriteBacks = " << C.getWB() << endl;
	
}
