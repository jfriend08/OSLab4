/*
Usage: ./iosched –s<schedalgo> <inputfile>
--	read in file
--	add and issue the first task
--	now all processes are correct for method i

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
int c, n1, n2, D_flag=0;
string sche, line;
int TIME=1, curTrack=0, issueCount=0, addCount=0, IsueTime=0;

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
		if(!Run_dq.empty())return 0; return -1;
	}

};

class IOsche{
public:
	vector < Job >* tasks;
	deque<Job> Run_dq;
	deque<Job> Finish_dq;
	Scheduler* scheduler;	
	int issuedT, run_idx;
	Job currentJob;
	
	void print(){
		// cout<<"tasks"<<endl;
		// for(int i=0; i<tasks->size(); i++){cout<<tasks->at(i).t<<" "<<tasks->at(i).track<<endl;}
		cout<<"run"<<endl;
		for(int i=0; i<Run_dq.size(); i++){cout<<Run_dq[i].t<<" "<<Run_dq[i].track<<endl;}
		cout<<"TIME:"<<TIME<<" curTrack:"<<curTrack<<" currentJob:"<<currentJob.t<<" "<<currentJob.track<<endl;
		// cout<<scheduler->select(tasks, Run_dq)<<endl;
	}
	void add(vector<Job>* tasks, deque<Job> &Run_dq){
		if(Run_dq.empty())TIME=tasks->front().t;
		printf("%d: %5d add %d\n",tasks->front().t, addCount, tasks->front().track); addCount++;
		Run_dq.push_back(tasks->front()); tasks->erase(tasks->begin());
		if(D_flag==1)print();
	}
	
	void issue(vector<Job>* tasks, deque<Job> &Run_dq){
		run_idx=scheduler->select(tasks, Run_dq);
		printf("%d: %5d issue %d %d\n",TIME, issueCount, Run_dq[run_idx].track, curTrack); issueCount++;
		TIME=TIME+abs(curTrack-Run_dq[run_idx].track); curTrack=Run_dq[run_idx].track;
		currentJob=Run_dq[run_idx];
		if(D_flag==1)print();

	}
	void finish(vector<Job>* tasks, deque<Job> &Run_dq){
		printf("%d: %5d finish %d\n",TIME, issueCount-1, TIME-Run_dq[run_idx].t);	
		Finish_dq.push_back(Run_dq[run_idx]);
		Run_dq.erase(Run_dq.begin()+run_idx);
		if(D_flag==1)print();
	}

	void change(){
		while (tasks->size()!=0 | Run_dq.size()!=0){			
			while(tasks->size()!=0 && tasks->front().t<=TIME){			
				add(tasks, Run_dq);
			}		
			if (addCount>1 ){
				finish(tasks, Run_dq);			
			}
			if(!Run_dq.empty()) issue(tasks, Run_dq);
			else if (Run_dq.empty()&& !tasks->empty()){add(tasks, Run_dq);issue(tasks, Run_dq);}
			// print();
			

		}
		


		// while(tasks->size()!=0 | Run_dq.size()!=0){
		// 	if(Run_dq.size()==0){
		// 		add(tasks, Run_dq);
		// 	}
		// 	else if(tasks->at(0).t<TIME){
		// 		add(tasks, Run_dq);	
		// 	}
		// 	issue(tasks, Run_dq);
		// 	print();
		// }
		
	}
	


};

int main(int argc, char *argv[]){
	IOsche IO;
	while((c=getopt(argc,argv,"s:D")) !=-1){
		switch (c){
			case 's':
				sche.assign(optarg);
				break;
			case 'D':
				D_flag=1;
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














