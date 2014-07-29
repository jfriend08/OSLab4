/*
Usage: ./iosched –s<schedalgo> <inputfile>
--	read in file
--	add and issue the first task



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
int TIME=1, curTrack=0, issueCount=0, addCount=0;

vector<int> tasks_tmp;

struct Job{
	int t;
	int track;
};
vector< Job > tasks_v;

class Scheduler{
public:
	virtual int select(vector<Job>* tasks, deque<Job>& Run_dq){return 0;}

};

class FIFO : public Scheduler{
public:
	int select(vector<Job>* tasks, deque<Job>& Run_dq){
		return 0;
	}

};

class IOsche{
public:
	vector < Job >* tasks;
	deque<Job> Run_dq;
	Scheduler* scheduler;	
	int issuedT;
	
	void print(){
		cout<<"tasks"<<endl;
		for(int i=0; i<tasks->size(); i++){cout<<tasks->at(i).t<<" "<<tasks->at(i).track<<endl;}
		cout<<"run"<<endl;
		for(int i=0; i<Run_dq.size(); i++){cout<<Run_dq[i].t<<" "<<Run_dq[i].track<<endl;}
		cout<<"TIME:"<<TIME<<" curTrack:"<<curTrack<<endl;
		// cout<<scheduler->select(tasks, Run_dq)<<endl;
	}
	void add(vector<Job>* tasks, deque<Job> &Run_dq){
		printf("%d: %5d add %3d\n",tasks->front().t,addCount, tasks->front().track); addCount++;
		Run_dq.push_back(tasks->front()); tasks->erase(tasks->begin());
	}
	
	void issue(vector<Job>* tasks, deque<Job> &Run_dq){
		int run_idx=scheduler->select(tasks, Run_dq);
		printf("%d: %5d issue %3d %d\n",TIME, run_idx,Run_dq[run_idx].track, curTrack);	
		TIME=TIME+Run_dq[run_idx].track; curTrack=Run_dq[run_idx].track;

	}

	void change(){
		while(tasks->size()!=0 | Run_dq.size()!=0){
			if(Run_dq.size()==0){
				add(tasks, Run_dq);
			}
			else if(tasks->at(0).t<Run_dq.front().t){
				add(tasks, Run_dq);	
			}
			issue(tasks, Run_dq);
			print();
		}
		
	}
	


};

int main(int argc, char *argv[]){
	IOsche IO;
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
				Job job_tmp;
				std::stringstream(line)>>n1>>n2;job_tmp.t=n1; job_tmp.track=n2;tasks_v.push_back(job_tmp);        		}					
		}		
	}
	fin.close();
	if (sche=="i"){IO.scheduler = new FIFO;}
	
	IO.tasks = &tasks_v;
	IO.change();


	return 0;
}














