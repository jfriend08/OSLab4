/*
Usage: ./iosched –s<schedalgo> <inputfile>
--	read in file
--	add and issue the first task
--	now all processes are correct for method i
--	now all process and SUM are correct for method i
--	now all process and SUM are correct for method i j s
--	now all process and SUM are correct for method i j s c

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
int c, n1, n2, D_flag=0, o_flag=0;
string sche, line;
int TIME=1, curTrack=0, issueCount=0, addCount=0, IsueTime=0;
int total_time=0, tot_movement=0, max_waittime=0;
double avg_turnaround=0, avg_waittime=0;
int direction=0, run_idx;

vector<int> tasks_tmp;

struct Job{
	int t;
	int track;
	int idx;
};
vector< Job > tasks_v;

struct Record{
	int arrive_t;
	int diskStart_t;
	int diskEnd_t;
};
vector< Record > Record_v;

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

class SSTF : public Scheduler{
public:
	int select(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx;
		if(!Run_dq.empty()){
			for (int i=0; i<Run_dq.size(); i++){				
				if( abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}
			}
			return shortest_idx;
		}
		return -1;
	}	
};

class SCAN : public Scheduler{
public:
	int searchleft(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx=-1;
		for (int i=0; i<Run_dq.size(); i++){				
			if( Run_dq[i].track <= curTrack && abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}			
		}		
		return shortest_idx;
	}
	int searchright(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx=-1;
		for (int i=0; i<Run_dq.size(); i++){				
			if( Run_dq[i].track >= curTrack && abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}				
		}
		return shortest_idx;
	}
	int select(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx; int reftmost=0; int rightmost=0;
		if(!Run_dq.empty()){
			if(direction ==0){				
				for (int i=0; i<Run_dq.size(); i++){				
					if( abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}				
				}
				if ((Run_dq[shortest_idx].track - curTrack) == 0 ){direction =0;}
				else if ((Run_dq[shortest_idx].track - curTrack) < 0 ){direction = -1;}
				else if ((Run_dq[shortest_idx].track - curTrack) > 0 ){direction = 1;}
			}			
			else if( direction == -1){				
				shortest_idx = searchleft(tasks, Run_dq);
				if (shortest_idx != -1) return shortest_idx;
				else if (shortest_idx == -1) {direction = 1; shortest_idx = searchright(tasks, Run_dq);}
			}
			else if( direction == 1){
				shortest_idx = searchright(tasks, Run_dq);
				if (shortest_idx != -1) return shortest_idx;
				else if (shortest_idx == -1) {direction = -1; shortest_idx = searchleft(tasks, Run_dq);}	
			}			
			return shortest_idx;
		}
		return -1;
	}	

};
class CSCAN : public Scheduler{
public:
	int searchright(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx=-1;
		for (int i=0; i<Run_dq.size(); i++){				
			if( Run_dq[i].track >= curTrack && abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}				
		}
		return shortest_idx;
	}
	int Toleftmost(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx=-1;
		for (int i=0; i<Run_dq.size(); i++){				
			if( Run_dq[i].track < shortest ){shortest = Run_dq[i].track; shortest_idx = i;}				
		}
		return shortest_idx;
	}
	
	int select(vector<Job>* tasks, deque<Job>& Run_dq){
		int shortest=10000; int shortest_idx; int reftmost=0; int rightmost=0;
		if(!Run_dq.empty()){
			if(direction ==0){				
				for (int i=0; i<Run_dq.size(); i++){				
					if( abs(Run_dq[i].track- curTrack) < shortest ){shortest = abs(Run_dq[i].track- curTrack); shortest_idx = i;}				
				}
				if ((Run_dq[shortest_idx].track - curTrack) == 0 ){direction =0;}
				else if ((Run_dq[shortest_idx].track - curTrack) > 0 ){direction = 1;}
			}			
			else if( direction == 1){
				shortest_idx = searchright(tasks, Run_dq);
				if (shortest_idx != -1) return shortest_idx;
				else if (shortest_idx == -1) {shortest_idx = Toleftmost(tasks, Run_dq);}	
			}			
			return shortest_idx;
		}
		return -1;
	}	

};

class IOsche{
public:
	vector < Job >* tasks;
	deque<Job> Run_dq;
	deque<Job> Finish_dq;
	Scheduler* scheduler;	
	int issuedT;
	Job currentJob;
	
	void print(){
		// cout<<"tasks"<<endl;
		// for(int i=0; i<tasks->size(); i++){cout<<tasks->at(i).t<<" "<<tasks->at(i).track<<endl;}
		// cout<<"run"<<endl;
		// for(int i=0; i<Run_dq.size(); i++){cout<<Run_dq[i].t<<" "<<Run_dq[i].track<<endl;}
		// cout<<"TIME:"<<TIME<<" curTrack:"<<curTrack<<" currentJob:"<<currentJob.t<<" "<<currentJob.track<<endl;
		// cout<<scheduler->select(tasks, Run_dq)<<endl;
		cout<<"run_idx:"<<run_idx<<endl;
	}
	void add(vector<Job>* tasks, deque<Job> &Run_dq){
		if(Run_dq.empty())TIME=tasks->front().t;
		if(o_flag==1)printf("%d: %5d add %d\n",tasks->front().t, tasks->front().idx, tasks->front().track); 
		// if(o_flag==1)printf("%d: %5d add %d\n",tasks->front().t, addCount, tasks->front().track); 
		Record record_tmp; record_tmp.arrive_t=tasks->front().t; Record_v.push_back(record_tmp);
		Run_dq.push_back(tasks->front()); tasks->erase(tasks->begin()); addCount++;
		if(D_flag==1)print();
	}
	
	void issue(vector<Job>* tasks, deque<Job> &Run_dq){
		run_idx=scheduler->select(tasks, Run_dq);
		if(o_flag==1)printf("%d: %5d issue %d %d\n",TIME, Run_dq[run_idx].idx, Run_dq[run_idx].track, curTrack); issueCount++;
		// if(o_flag==1)printf("%d: %5d issue %d %d\n",TIME, issueCount, Run_dq[run_idx].track, curTrack); issueCount++;
		Record_v[Run_dq[run_idx].idx].diskStart_t=TIME;
		// Record_v[issueCount-1].diskStart_t=TIME;
		if(curTrack!=Run_dq[run_idx].track)tot_movement=tot_movement + abs(curTrack - Run_dq[run_idx].track);
		TIME=TIME+abs(curTrack-Run_dq[run_idx].track); curTrack=Run_dq[run_idx].track;
		currentJob=Run_dq[run_idx];
		if(D_flag==1)print();

	}
	void finish(vector<Job>* tasks, deque<Job> &Run_dq){
		if(o_flag==1)printf("%d: %5d finish %d\n",TIME, Run_dq[run_idx].idx, TIME-Run_dq[run_idx].t);	
		// if(o_flag==1)printf("%d: %5d finish %d\n",TIME, issueCount-1, TIME-Run_dq[run_idx].t);	
		Record_v[Run_dq[run_idx].idx].diskEnd_t=TIME;
		// Record_v[issueCount-1].diskEnd_t=TIME;
		Finish_dq.push_back(Run_dq[run_idx]);
		Run_dq.erase(Run_dq.begin()+run_idx);
		if(D_flag==1)print();
	}
	void SUM(){
		if(o_flag==1) cout<<"IOREQS INFO"<<endl;
		total_time=Record_v[Record_v.size()-1].diskEnd_t; 
		// tot_movement=total_time-1;
		for (int i=0; i<Record_v.size(); i++){			
			avg_turnaround = avg_turnaround + (Record_v[i].diskEnd_t - Record_v[i].arrive_t);
			double waittime=Record_v[i].diskStart_t - Record_v[i].arrive_t;
			avg_waittime = avg_waittime + waittime;
			if(waittime>max_waittime)max_waittime=waittime;
			if(o_flag==1) printf("%5d: %5d %5d %5d\n",i, Record_v[i].arrive_t, Record_v[i].diskStart_t, Record_v[i].diskEnd_t);
		}
		avg_turnaround=avg_turnaround/Record_v.size();
		avg_waittime=avg_waittime/Record_v.size();

		printf("SUM: %d %d %.2f %.2f %d\n",TIME, tot_movement, avg_turnaround, avg_waittime, max_waittime);

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
		}
		SUM();		
	}
	


};

int main(int argc, char *argv[]){
	IOsche IO;
	while((c=getopt(argc,argv,"s:D:o")) !=-1){
		switch (c){
			case 's':
				sche.assign(optarg);
				break;
			case 'D':
				D_flag=1;
				break;
			case 'o':
				o_flag=1;
				break;
			default:
				abort();
		}
	}
	// cout <<"sche:"<<sche<<endl;
	if(sche==""){cout<<"please specify schedule method"<<endl; return 0;}

	ifstream fin (argv[argc-1]);
	if (!fin.is_open()){cout<<"Cannot open the file0"<<endl;	}
	else{
		int add=0;
		while(!fin.eof()){
			getline(fin, line);
			if (line[0]!='#' && line !=""){		
				Job job_tmp;
				std::stringstream(line)>>n1>>n2;job_tmp.t=n1; job_tmp.track=n2; job_tmp.idx = add;tasks_v.push_back(job_tmp);        		
				add++;
			}					
		}		
	}
	fin.close();
	
	if (sche=="i"){IO.scheduler = new FIFO;}
	else if (sche=="j"){IO.scheduler = new SSTF;}
	else if (sche=="s"){IO.scheduler = new SCAN;}
	else if (sche=="c"){IO.scheduler = new CSCAN;}
	
	IO.tasks = &tasks_v;
	IO.change();


	return 0;
}














