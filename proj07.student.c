/*
Andrew Gilbertson gilbe244@cse410
2017-03-27
proj07.student.c

Your Comments
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <deque>
#include <algorithm>    
#include <iterator>

using std::cout; using std::endl;

//its not REALLY a page I suppose, helps the thought process
struct page{
	int valid;
	int referenced;
	int modified;
	int frame;
};




//a simple function to print the page table
void print_vector(std::vector<page> table){
	cout<<"Page table "<<endl;
	cout<<"I"<<" V"<<" R"<<" M"<<" F"<<endl;
	for(unsigned int i = 0; i < 16;i++)
	{
		cout<<std::hex<<i<<" "<<table[i].valid<<" ";
		cout<<table[i].referenced<<" "<<table[i].modified<<" ";
		cout<<std::hex<<std::setw(2)<<std::setfill('0')<<table[i].frame<<endl;
		
	}
	cout<<endl;

	
	
}

int main(int argc, char *argv[]){
	
	//declaring all necessary variables
	char first;
	std::string test;
	std::string temp;
	std::ifstream doc;
	doc.open(argv[2]);
	std::string line;
	std::deque<int> fifo_queue;
	std::vector<page> page_table;
	std::deque<int> lru_queue;
	std::deque<int> clock_queue;
	
	//populating the initial page table
	for (int i = 0; i <16; i++){
		page temp;
		temp.valid = 0;
		temp.referenced = 0;
		temp.modified = 0;
		temp.frame = 0;
		page_table.push_back(temp);
		
	}	
	//turn arg into number
	int n = atoi(argv[1]);
		
	//if given a negative number (not allowed) turn into 0 to account
	if(n <0)
		n = 0;
	
	//print if not 0
	if(n!=0){
	print_vector(page_table);
}
	//initialize counts
	int loop_count =0;
	int write_count = 0;
	int read_count = 0;
	int queue_index = 0;
	int page_alloc = 0;
	int clock_pointer = 0;
	
	
	//setting replacement type 
	getline(doc,line);
	bool FIFO = false;
	bool LRU = false;
	bool Clock = false;
	bool page_fault = false;
	bool write_back = false;

	
	//if statements to set a true flag;
	if(line == "FIFO"){
		FIFO = true;
	}
	else if(line =="LRU"){
		cout<<"LRU"<<endl;
		LRU = true;
	}
	else if(line == "Clock"){
		cout<<"Clock"<<endl;
		Clock = true;
	}
	else{
		cout<<"Not a valid replacement type, setting replacement type to FIFO"<<endl;
		FIFO = true;
	}
	temp = line;
	
	//have to get page frame limit
	getline(doc,line);
	int limit = atoi(line.c_str());
	
	std::deque<int> frames;
	
	for(int i = 0x40; i < 0x40+limit;i++)
	{
		frames.push_back(i);
	}
	
	//processing the lines and outputting as necessary
	while (std::getline(doc,line))
	{

		//converting lines into required variables
		std::istringstream check(line);
		check >> first >> test;
		std::string hex = "0x";
		std::string con;
		con = hex+test;
		
		//quick checks to incremenet count
		if(first == 'R')
		{
			read_count++;
		}
		if(first == 'W')
		{
			write_count++;
		}
		
		//different variables to store different values as required
		unsigned int logical = std::stoi(con,NULL,16);
		unsigned int offset = std::stoi(con,NULL,16);
		unsigned int page_number = std::stoi(con,NULL,16);
		
		//offsetting the bits to make them work
		page_number = (page_number>>12);
		offset= (offset<<20)>>20;
		//the first bit of processing, to hand out the initial frames
		//takes advantage of size requirements for queue
		if(page_alloc <= limit-1)
		{
			//fifo processesing
			if(FIFO == true)
			{
				//if the page number isn't already allocated
				if (std::find(fifo_queue.begin(),fifo_queue.end(),page_number) == fifo_queue.end())
				{
					if(first == 'W')
						page_table[page_number].modified = 1;
					page_table[page_number].valid = 1;
					page_table[page_number].referenced = 1;
					page_table[page_number].frame = frames[0];
					frames.pop_front();	
					fifo_queue.push_back(page_number);
					page_alloc++;
					page_fault = true;
				}
				//pif it is, have to change modified bit 
				else{
					if (first == 'W')
						page_table[page_number].modified = 1;
				}
			}
			//LRU processing, initial (should be same as FIFO, as there are open frames
			else if(LRU == true)
			{
				//checking if page number is in deque holding valid pages
				if (std::find(lru_queue.begin(),lru_queue.end(),page_number) == lru_queue.end())
				{
					if(first == 'W')
						page_table[page_number].modified = 1;
					page_table[page_number].valid = 1;
					page_table[page_number].referenced = 1;
					page_table[page_number].frame= frames[0];
					frames.pop_front();
					lru_queue.push_back(page_number);
					page_alloc++;
					page_fault = true;
					
				}
				else{
					//find the referenced page, set it in the back
					std::deque<int>::iterator i = lru_queue.begin();
					i = find(lru_queue.begin(),lru_queue.end(),page_number);
					lru_queue.erase(i);
					lru_queue.push_back(page_number);
					if(first =='W')
						page_table[page_number].modified = 1;
	
				}
	
				
				
			}
			//clock algorithm
			else if (Clock == true){

				if (std::find(clock_queue.begin(),clock_queue.end(),page_number) == clock_queue.end())
				{
					if(first == 'W')
						page_table[page_number].modified = 1;
					page_table[page_number].valid = 1;
					page_table[page_number].referenced = 1;
					page_table[page_number].frame = frames[0];
					frames.pop_front();	
					clock_queue.push_back(page_number);
					clock_pointer++;
					page_alloc++;
					page_fault = true;	
					
				}
				
				else{
					if (first == 'W')
						page_table[page_number].modified = 1;
					
				}
				
			}

		}
		
		
		else {
			
			//FIFO method processing
			if(FIFO == true)
			{
				if (std::find(fifo_queue.begin(),fifo_queue.end(),page_number) == fifo_queue.end())
				{
					for (int i = 0; i < 16; i++)
					{
						//finds the matching frame with the spot in the fifo queue
						if (page_table[i].frame == page_table[fifo_queue[queue_index%limit]].frame)
					
						{
							if(page_table[i].modified == 1)
								write_back = true;
							if (first == 'W')
								page_table[page_number].modified = 1;
							//unvalidates the old spot, re assigns frame to new page index
							page_table[i].valid = 0;
							page_table[page_number].valid = 1;
							page_table[page_number].referenced = 1;
							page_table[page_number].frame = page_table[fifo_queue[queue_index%limit]].frame;
							fifo_queue[queue_index%limit] = page_number;
							//increments the index, so when % the queue is "circular"
							queue_index++;
							page_fault = true;
							break;

						
						}
					
					
					}
			}
			
			else{
				if (first == 'W')
					page_table[page_number].modified = 1;

				
			}
			
		}
		//LRU processing after all initial frames have been allocated
		else if(LRU == true)
		{
				if (std::find(lru_queue.begin(),lru_queue.end(),page_number) == lru_queue.end())
				{
					if(first =='W')
						page_table[page_number].modified = 1;

					if(page_table[lru_queue[0]].modified == 1)
						write_back = true;
					page_table[lru_queue[0]].valid = 0;
					page_table[page_number].valid = 1;
					page_table[page_number].referenced = 1;
					page_table[page_number].frame = page_table[lru_queue[0]].frame;
					lru_queue.pop_front();
					lru_queue.push_back(page_number);
					page_fault = true;

					
				}
				else{
					std::deque<int>::iterator i = lru_queue.begin();
					i = find(lru_queue.begin(),lru_queue.end(),page_number);
					lru_queue.erase(i);
					lru_queue.push_back(page_number);
					if (first == 'W')
					{
					page_table[page_number].modified = 1;
					
					}
					
				}

			
		}
			
		//clock algorithm
		else if(Clock == true)
		{
			//searches to see if its in
			if (std::find(clock_queue.begin(),clock_queue.end(),page_number) == clock_queue.end())
			{
				//checks the reference bit, setting it to zero as it goess
				while(page_table[clock_queue[clock_pointer%limit]].referenced!=0)
				{
					page_table[clock_queue[clock_pointer%limit]].referenced = 0;
					clock_pointer++;
					
					
				}
				if(first =='W')
					page_table[page_number].modified = 1;
				//write back flag
				if (page_table[clock_queue[clock_pointer%limit]].modified == 1)
					write_back = true;
				page_table[clock_queue[clock_pointer%limit]].valid = 0;
				page_table[page_number].valid = 1;
				page_table[page_number].referenced = 1;
				page_table[page_number].frame = page_table[clock_queue[clock_pointer%limit]].frame;
				clock_queue[clock_pointer%limit] = page_number;
				page_fault = true;
				
			}
			else{
				page_table[page_number].referenced = 1;
				if (first =='W')
					page_table[page_number].modified =1;
				
				
			}
			
			
			
		}
		
		
		}
		
	if (write_back){
		

		if (!page_fault)
			{
			//std::hex will turn an int into hex equivalent for output 
			cout<<first<<" "<<std::hex<<std::setw(4)<<std::setfill('0');
			cout<<logical<<" "<<std::hex<<page_number<<" "<<std::hex;
			cout<<std::setw(3)<<std::setfill('0')<<offset<<" WB"<<" "<<page_table[page_number].frame<<std::setw(3)<<std::setfill('0')<<offset<<endl;
			loop_count++;
			}
		
		else{
			//std::hex will turn an int into hex equivalent for output 
			cout<<first<<" "<<std::hex<<std::setw(4)<<std::setfill('0');
			cout<<logical<<" "<<std::hex<<page_number<<" "<<std::hex;
			cout<<std::setw(3)<<std::setfill('0')<<offset;
			cout<<" F"<<" WB"<<" "<<page_table[page_number].frame<<std::setw(3)<<std::setfill('0')<<offset<<endl;
			loop_count++;
			
		}
	}
	
	else{
		if (!page_fault)
			{
			//std::hex will turn an int into hex equivalent for output 
			cout<<first<<" "<<std::hex<<std::setw(4)<<std::setfill('0');
			cout<<logical<<" "<<std::hex<<page_number<<" "<<std::hex;
			cout<<std::setw(3)<<std::setfill('0')<<offset<<" "<<page_table[page_number].frame<<std::setw(3)<<std::setfill('0')<<offset<<endl;
			loop_count++;
			}
		
		else{
			//std::hex will turn an int into hex equivalent for output 
			cout<<first<<" "<<std::hex<<std::setw(4)<<std::setfill('0');
			cout<<logical<<" "<<std::hex<<page_number<<" "<<std::hex;
			cout<<std::setw(3)<<std::setfill('0')<<offset;
			cout<<" F"<<" "<<page_table[page_number].frame<<std::setw(3)<<std::setfill('0')<<offset<<endl;
			loop_count++;
			
		}
	}
	
	
		//a way of printing every N times 
		if(n!=0  && loop_count % n == 0 )
		{
			print_vector(page_table);
		}
		
		page_fault = false;
		write_back = false;
	}
	
	//quick check to make sure it wasn't just printed
	if(n!=0&& loop_count % n!=0  )
	{
		print_vector(page_table);
	}
	
	//printing all the counts
	cout<<endl;
	cout<<"Type: "<<temp<<endl;
	cout<<"Allocated frames: "<<limit<<endl;
	cout<<"Total references: "<<loop_count<<endl;
	cout<<"Read Ops: "<<read_count<<endl;
	cout<<"Write Ops: "<<write_count<<endl;
	



}

