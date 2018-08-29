#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;


typedef struct mem_s{
	char *buff;
}mem;
int main(){

	mem mm;
	mm.buff=new char(100);
	memcpy(mm.buff,"9999",4);
	mem mm2;
	memcpy(&mm2,&mm,sizeof(mem));
	cout << "mm2 : "<<mm2.buff<<endl;
	delete[] mm2.buff;
	mm2.buff = NULL;
	cout << "mm : "<<mm.buff<<endl;
	char ff=0x19;
	cout <<"ff"<<ff<<endl;
	printf("%d",ff);
	return 0;
}
