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

Table::Table(int ways, int words, int block_size,std::string method,bool totally){
	//reserve the memory for less painfull blocks append
	this->ways.reserve(ways);
	this->words = words;
	this->block_size = block_size;
	this->words_per_way=words/ways;
	this->blocks_per_way=words_per_way/block_size;
	this->method = method;
	this->is_associative = totally;
	transform(this->method.begin(),this->method.end(),this->method.begin(),::toupper);
	//Fill the "table" with "empty" vectors
	for(int i=0;i<ways;i++){
		std::vector<Block> temp;
		temp.reserve(blocks_per_way);
		for(int j=0;j<blocks_per_way;j++){
			//Block temp(block_size);
			temp.push_back(Block(block_size));
		}
		this->ways.push_back(temp);
	}
	//Replacement functions info
	if(!totally){
		this->rep_info.reserve(blocks_per_way);
		for(int i=0;i<blocks_per_way;i++){
			this->rep_info.push_back(Info(ways));
		}		
	}
	else{
		this->associative_info.init(ways,this->blocks_per_way);
	}

}

//Return a vector of integer with the following structure
//{block_address,way}
void Table::insert_value(int value){
	this->total++;
	//Necessary values
	int temp_value = value/this->block_size;
	int block_address = temp_value%this->blocks_per_way;
	int position = value%this->block_size;
	bool found = false;
	//Check if the value is in any way
	if(!this->is_associative){
		int index = 0;
		
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
		this->history.push_back(Element(value,temp_value,block_address,position,index,found));		
	}
	else{
       	int ind1=0;
       	int ind2=0;
       	while(ind1 < this->blocks_per_way && !found){
           	ind2=0;
           	while(ind2<this->ways.size() && !found){
               	if(value == this->ways[ind2][ind1].position[position]){
                   	found = true;
                   	this->hits++;
                   	ind2--;
                   	ind1--;
               	}
               	ind2++;
           }
           ind1++;
       	}
       	
       	if(!found){
        	std::tuple<int,int> tup = this->all_occuped_as();
        	if(std::get<0>(tup) == -1){
            	tup = ((*this).*as_rep_funct[this->method])();
        	}
        	this->fill_block(this->ways[std::get<1>(tup)][std::get<0>(tup)],position,value);
        	this->update_as_info(std::get<0>(tup),std::get<1>(tup),true);
       	}
       	else{
        	this->update_as_info(ind1,ind2,false);
       	}
       	this->history.push_back(Element(value,-1,ind2,position,ind1,found));
	}

	/* Create for a more decent table */
	
	/* ----------------------------- */
}

//Check if the block is occuped in all ways
//either way, return the index of the way with free block
int Table::all_occuped(int address){
	for(int i=0;i<this->ways.size();i++){
		if(ways[i][address].position[0] == -1)
			return i;
	}
	return -1;
}

void Table::change_method(std::string new_method){
	this->method = new_method;
	transform(this->method.begin(),this->method.end(),this->method.begin(),::toupper);
}

void Table::update_info(int ad, int w,bool replacing){
	//Inserting order
	std::vector<int> &order = this->rep_info[ad].element_order;
	std::vector<int> &frecuency = this->rep_info[ad].frecuency;
	std::vector<int> &used = this->rep_info[ad].used_order;
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
	std::vector<Element> &t = this->history;
	std::cout << "ref\t" << "dir1\t" << "dir2\t" << "dir3\t" << "way\t" << "hit\n";
	for(int i=0;i<this->history.size();i++){
		std::cout << t[i].refi << "\t" << t[i].dir1 << "\t" << t[i].dir2 << "\t" << t[i].dir3 << "\t" << t[i].way << "\t" << t[i].hit << "\n";
	}
}

/** Totally Associative Functions **/

/** --- Max/Min element of matrix **/
int Table::max_as_element(std::vector<std::vector<int> > &v){
    int current = 0;
    for(unsigned int i=0;i<v.size();i++){
        for(unsigned int j=0;j<v[i].size();j++){
            if(v[i][j] > current)
                current = v[i][j];
        }
    }
    return current;
}

int Table::min_as_element(std::vector<std::vector<int> > &v){
    int current = INT_MAX;
    for(unsigned int i = 0;i<v.size();i++){
        for(unsigned int j=0;j<v[i].size();j++){
            if(v[i][j] < current){
                current = v[i][j];
            }
        }
    }
    return current;
}

void Table::update_as_info(int pos, int w, bool replacing){
    //Inserting order
    std::vector<std::vector<int> > &order = this->associative_info.element_order;
    std::vector<std::vector<int> > &frecuency = this->associative_info.frecuency;
    std::vector<std::vector<int> > &used = this->associative_info.used_order;
    int max_v = max_as_element(order);
    //All blocks filled
    if(max_v == this->blocks_per_way * this->ways.size()){
        for(int i=0;i<this->blocks_per_way;i++){
            for(int j=0;j<this->ways.size();j++){
                if(replacing && order[i][j] > order[pos][w]){
                    order[i][j]--;
                }
                if(used[i][j] > used[pos][w]){
                    used[i][j]--;
                }
            }

        }
        if(replacing) order[pos][w] = max_v;
        used[pos][w] = max_v;
    }
    else{
        if(replacing) order[pos][w] = max_v+1;
        used[pos][w] = max_v+1;
    }
    //Used order

    //Frecuency order
    if(replacing){
        frecuency[pos][w] = 1;
    }
    else{
        frecuency[pos][w]++;
    }
}


std::tuple<int,int> Table::all_occuped_as(){
   	for(int i=0;i<this->blocks_per_way;i++){
       	for(int j=0;j<this->ways.size();j++){
           	if(this->ways[j][i].position[0] == -1) return std::make_tuple(i,j);
       	}
   	}
   return std::make_tuple(-1,-1);
}

std::tuple<int,int> Table::as_random(){
    time_t sec;
    time(&sec);
    srand((unsigned int) sec);
    return std::make_tuple(rand()%this->blocks_per_way,rand()%this->ways.size());

}
std::tuple<int,int> Table::as_fifo(){
    for(int i=0;i<this->blocks_per_way;i++){
        for(int j=0;j<this->ways.size();j++){
            if(this->associative_info.element_order[i][j] == 1)
                return std::make_tuple(i,j);
        }

    }
    return std::make_tuple(0,0);

}

std::tuple<int,int> Table::as_lifo(){
    for(int i=0;i<this->blocks_per_way;i++){
        for(int j=0;j<this->ways.size();j++){
            if(this->associative_info.element_order[i][j] == this->blocks_per_way*this->ways.size())
                return std::make_tuple(i,j);
        }
    }
    return std::make_tuple(0,0);

}

std::tuple<int,int> Table::as_lfu(){
    int min_value = min_as_element(this->associative_info.frecuency);
    for(int i=0;i<this->blocks_per_way;i++){
        for(int j=0;j<this->ways.size();j++){
            if(this->associative_info.frecuency[i][j] == min_value)
                return std::make_tuple(i,j);
        }

    }
    return std::make_tuple(0,0);

}
std::tuple<int,int> Table::as_lru(){
    for(int i=0;i<this->blocks_per_way;i++){
        for(int j=0;j<this->ways.size();j++){
            if(this->associative_info.used_order[i][j] == 1)
                return std::make_tuple(i,j);
        }

    }
    return std::make_tuple(0,0);

}
