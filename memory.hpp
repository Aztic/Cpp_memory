#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#define DEFAULT_VALUE -1
using namespace std;

class Table{
private:
  	typedef int (Table::*ReplaceFunction)(int);
  /* ------------- Data Structures ------------- */
	//blocks structure
	struct Block{
		vector<int> position;
		Block(int quantity){
			this->position.reserve(quantity);
			for(int i=0;i<quantity;i++){
				this->position.push_back(DEFAULT_VALUE);
			}
		}
	};
	//Print info
	/*
	  | Ref | dir1 | dir2 | dir3 | way | hit |
	  ----------------------------------------
	  |.....|......|......|......|.....|.....|
	*/
	struct Element{
		unsigned int refi;
		int dir1;
    	int dir2;
    	int dir3;
    	int way;
    	bool hit;
    	Element(unsigned int r,int d1, int d2, int d3, int w, bool h){
	      this->refi = r;
	      this->dir1 = d1;
	      this->dir2 = d2;
	      this->dir3 = d3;
	      this->way = w;
	      this->hit = h;
	    }
  	};
  //Replacement politics info
  	struct Info{
	    vector<int> element_order;
	    vector<int> used_order;
	    vector<int> frecuency;
	    Info(int ways){
	      this->element_order.resize(ways);
	      this->used_order.resize(ways);
	      this->frecuency.resize(ways);
	    }
	};
  /* ------------------------------------------- */

  /* ------------ Functions/Procedures---------- */
	void fill_block(Block &b, int position, int value);
	int all_occuped(int address);
	void change_method(string new_method);
	int random(int index);
	int lfu(int index);
	int lru(int index);
	int fifo(int index);
	int lifo(int index);
	void update_info(int ad,int w,bool replacing);
  /* ------------------------------------------- */


  /*-------------- Data section ---------------- */
	vector<vector<Block> > ways;
	string mode;
	int words;
	int block_size;
	int words_per_way;
	int blocks_per_way;
  	int total=0;
	int hits=0;
  	vector<Info> rep_info;
 	vector<Element> history;
  	string method;
 	map<string,ReplaceFunction> rep_funct={
 	 	{"RANDOM",&Table::random},
 	 	{"LFU",&Table::lfu},
 	 	{"LRU",&Table::lru},
 	 	{"FIFO",&Table::fifo},
 	 	{"LIFO",&Table::lifo}
 	};
  /* ------------------------------------------- */

public:
	Table();
	Table(int ways,int words, int block_size,string method);
	void insert_value(int value);
  	double hit_percent();
  	double fail_percent();
  	void print_table();
};

#endif
