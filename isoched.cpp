/*
Usage: ./iosched –s<schedalgo> <inputfile>
--	read in file

*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector> 
#include <list>
#include <sstream>
#include <iterator>
#include <map> 
#include <iomanip>
#include <utility> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits>
#include <queue>


using namespace std;
int c, n1, n2;
string sche, line;
vector< vector<int> > tasks_v;
vector<int> tasks_tmp;

int main(int argc, char *argv[]){

	while((c=getopt(argc,argv,"s:")) !=-1){
		switch (c){
			case 's':
				sche.assign(optarg);
				break;
			default:
				abort();
		}
	}
	cout <<"sche:"<<sche<<endl;
	if(sche==""){cout<<"please specify schedule method"<<endl; return 0;}

	ifstream fin (argv[argc-1]);
	if (!fin.is_open()){cout<<"Cannot open the file0"<<endl;	}
	else{
		while(!fin.eof()){
			getline(fin, line);
			if (line[0]!='#' && line !=""){				
				std::stringstream(line)>>n1>>n2;tasks_tmp.push_back(n1);tasks_tmp.push_back(n2);
        		tasks_v.push_back(tasks_tmp);tasks_tmp.clear();
			}					
		}		
	}
	fin.close();


	return 0;
}
