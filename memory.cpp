#include "memory.hpp"

void Table::fill_block(Block &b,int position, int value){

	int current = value - position;
	for(int i=0;i<b.position.size();i++){
		b.position[i] = current;
		current++;
	}
}

Table::Table(){
	this->words = 0;
	this->block_size = 0;
	this->words_per_way=0;
}

Table::Table(int ways, int words, int block_size,string method){
	//reserve the memory for less painfull blocks append
	this->ways.reserve(ways);
	this->words = words;
	this->block_size = block_size;
	this->words_per_way=words/ways;
	this->blocks_per_way=words_per_way/block_size;
  	this->method = method;
  	transform(this->method.begin(),this->method.end(),this->method.begin(),::toupper);
	//Fill the "table" with "empty" vectors
	for(int i=0;i<ways;i++){
    		vector<Block> temp;
		temp.reserve(blocks_per_way);
		for(int j=0;j<blocks_per_way;j++){
			//Block temp(block_size);
			temp.push_back(Block(block_size));
		}
    this->ways.push_back(temp);
	}
  //Replacement functions info
  this->rep_info.reserve(blocks_per_way);
  for(int i=0;i<blocks_per_way;i++){
    this->rep_info.push_back(Info(ways));
  }
}

void Table::insert_value(int value){
  this->total++;
  //Necessary values
  int temp_value = value/this->block_size;
  int block_address = temp_value%this->blocks_per_way;
  int position = value%this->block_size;
  //Check if the value is in any way
  int index = 0;
  bool found = false;
  while(index < this->ways.size() && !found){
    if(value == this->ways[index][block_address].position[position]){
      this->hits++;
      found = true;
    }
    index++;
  }
  if(!found){
    index = this->all_occuped(block_address);
    if(index == -1){
      //Replace
      index = ((*this).*rep_funct[this->method])(block_address);
    }
    this->fill_block(this->ways[index][block_address],position,value);
    this->update_info(block_address,index,true);
  }
  else{
  	index--;
    this->update_info(block_address,index,false);
  }
  /* Create for a more decent table */
  this->history.push_back(Element(value,temp_value,block_address,position,index,found));
  /* ----------------------------- */
}

//Check if the block is occuped int all ways
//either way, return the index of the way with free block
int Table::all_occuped(int address){
  for(int i=0;i<this->ways.size();i++){
    if(ways[i][address].position[0] == -1)
      return i;
  }
  return -1;

}

void Table::change_method(string new_method){
  this->method = new_method;
  transform(this->method.begin(),this->method.end(),this->method.begin(),::toupper);
}

void Table::update_info(int ad, int w,bool replacing){
  //Inserting order
  vector<int> &order = this->rep_info[ad].element_order;
  vector<int> &frecuency = this->rep_info[ad].frecuency;
  vector<int> &used = this->rep_info[ad].used_order;
  int max_v = *max_element(order.begin(),order.end());
  //All ways filled
  /*Assumming a cache of 5 ways with the following inserting order
  [1,4,3,5,2] and inserting element in the way 3, this'll be the procedure
  -rest all values higher than the selected way value by 1
  [1,3,2,4,2]
  -Replace the way value with the higher possible value
  [1,3,2,4,5]*/
  if(max_v == this->ways.size()){
    for(int i=0;i<this->ways.size();i++){
      if(replacing && order[i] > order[w]){
        order[i]--;
      }
      if(used[i] > used[w]){
        used[i]--;
      }
    }
    if(replacing) order[w] = max_v;
    used[w] = max_v;
  }
  else{
    if(replacing) order[w] = max_v+1;
    used[w] = max_v+1;
  }
  //Used order

  //Frecuency order
  if(replacing){
    frecuency[w] = 1;
  }
  else{
    frecuency[w]++;
  }
}

//Replacement functions
int Table::random(int index){
  time_t sec;
  time(&sec);
  srand((unsigned int) sec);
  return rand()%this->ways.size();
}

int Table::lfu(int index){
  int min_value = *min_element(this->rep_info[index].frecuency.begin(),this->rep_info[index].frecuency.end());
  for(int i=0;i<this->ways.size();i++){
    if(this->rep_info[index].frecuency[i] == min_value)
      return i;
  }
}
int Table::lru(int index){
  for(int i=0;i<this->ways.size();i++){
    if(this->rep_info[index].used_order[i] == 1)
      return i;
  }
}
int Table::fifo(int index){
  for(int i=0;i<this->ways.size();i++){
    if(this->rep_info[index].element_order[i] == 1)
      return i;
  }
}
int Table::lifo(int index){
  for(int i=0;i<this->ways.size();i++){
    if(this->rep_info[index].element_order[i] == this->ways.size())
      return i;
  }
}
//End of replacement functions
double Table::hit_percent(){
  return (double)this->hits/(double)this->total;
}

double Table::fail_percent(){
  return 1-this->hit_percent();
}

void Table::print_table(){
	vector<Element> &t = this->history;
	cout << "ref\t" << "dir1\t" << "dir2\t" << "dir3\t" << "way\t" << "hit\n";
	for(int i=0;i<this->history.size();i++){
		cout << t[i].refi << "\t" << t[i].dir1 << "\t" << t[i].dir2 << "\t" << t[i].dir3 << "\t" << t[i].way << "\t" << t[i].hit << "\n";
	}
}
