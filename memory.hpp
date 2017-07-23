#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include <tuple>
#include <climits>
#define DEFAULT_VALUE -1


class Table{
private:
  	typedef int (Table::*ReplaceFunction)(int);
  	typedef std::tuple<int,int> (Table::*AsReplaceFunction)(void);
  /* ------------- Data Structures ------------- */
	//blocks structure
	struct Block{
		std::vector<int> position;
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
	    std::vector<int> element_order;
	    std::vector<int> used_order;
	    std::vector<int> frecuency;
	    Info(int ways){
	      this->element_order.resize(ways);
	      this->used_order.resize(ways);
	      this->frecuency.resize(ways);
	    }
	};

	struct AsInfo{
		std::vector<std::vector<int> > element_order;
		std::vector<std::vector<int> > used_order;
		std::vector<std::vector<int> > frecuency;
		void init(int w,int blocks){
			this->element_order.resize(blocks);
			this->used_order.resize(blocks);
			this->frecuency.resize(blocks);
			for(int i=0;i<blocks;i++){
				this->element_order[i].resize(w);
				this->used_order[i].resize(w);
				this->frecuency[i].resize(w);
			}
		}
	};
  /* ------------------------------------------- */

  /* ------------ Functions/Procedures---------- */
	void fill_block(Block &b, int position, int value);
	int all_occuped(int address);
	void change_method(std::string new_method);
	//No Associative functions
	int random(int index);
	int lfu(int index);
	int lru(int index);
	int fifo(int index);
	int lifo(int index);
	void update_info(int ad,int w,bool replacing);

	//Associative functions
	std::tuple<int,int> as_random();
	std::tuple<int,int> as_lfu();
	std::tuple<int,int> as_lru();
	std::tuple<int,int> as_fifo();
	std::tuple<int,int> as_lifo();
	void update_as_info(int ad,int w,bool replacing);
	std::tuple<int,int> all_occuped_as();
	int max_as_element(std::vector<std::vector<int> > &v);
	int min_as_element(std::vector<std::vector<int> > &v);
  /* ------------------------------------------- */


  /*-------------- Data section ---------------- */
	std::vector<std::vector<Block> > ways;
	std::string mode;
	int words;
	int block_size;
	int words_per_way;
	int blocks_per_way;
  	int total=0;
	int hits=0;
	bool is_associative;
  	std::vector<Info> rep_info;
 	std::vector<Element> history;
 	AsInfo associative_info;
  	std::string method;
  	//No-associative replacement functions
 	std::map<std::string,ReplaceFunction> rep_funct={
 	 	{"RANDOM",&Table::random},
 	 	{"LFU",&Table::lfu},
 	 	{"LRU",&Table::lru},
 	 	{"FIFO",&Table::fifo},
 	 	{"LIFO",&Table::lifo}
 	};

 	//Associative replacement functions
 	std::map<std::string,AsReplaceFunction> as_rep_funct ={
 		{"RANDOM",&Table::as_random},
 		{"LFU",&Table::as_lfu},
 		{"LRU",&Table::as_lru},
 		{"FIFO",&Table::as_fifo},
 		{"LIFO",&Table::as_lifo}
 	};
  /* ------------------------------------------- */

public:
	Table();
	Table(int ways,int words, int block_size,std::string method,bool totally);
	void insert_value(int value);
  	double hit_percent();
  	double fail_percent();
  	void print_table();
};

#endif
