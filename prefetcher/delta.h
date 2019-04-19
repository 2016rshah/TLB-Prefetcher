#ifndef __DELTA_H__
#define __DELTA_H__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "champsim.h"

typedef struct predicted_distance {
	uint64_t distance;
	uint64_t lru;
	
	predicted_distance(uint64_t d) : distance(d), lru(0){}
}p_distance;

class Delta {
	// TODO: the predicted values are supposed to be managed with LRU
	uint64_t S;
	std::unordered_map<uint64_t, std::vector<p_distance*> > d_table;
	uint64_t last_accessed_page;
	uint64_t previous_distance;
	uint64_t current_distance;
	uint64_t current_page;
	
	private:
		uint64_t get_page_addr(uint64_t full_addr) {
			uint64_t page = (full_addr >> LOG2_PAGE_SIZE) << LOG2_PAGE_SIZE;
			return page;	
		}

		void set_current_values(uint64_t full_addr) {
			uint64_t page = get_page_addr(full_addr);
			current_page = page;
			current_distance = (page - last_accessed_page);
		}
		
		int find_distance_in_prev() {
			int size = d_table[previous_distance].size();
			for (int i = 0; i < size; i++) {
				if (d_table[previous_distance][i]->distance == current_distance) {
					return i;
				}
			}
			return -1;
		}
		
		void lru_update(int index) {
			int max = d_table[previous_distance].size();
			for (int i = 0; i < max; i++) {
				d_table[previous_distance][i]->lru++;
			}
			d_table[previous_distance][index]->lru = 0;
		}

		// only call when lru is full
		int lru_evict() {
			for (uint64_t i = 0; i < S; i++) {
				if (d_table[previous_distance][i]->lru == S - 1) {
					// we've found a value to evict, replace with current
					d_table[previous_distance][i]->distance = current_distance;
					d_table[previous_distance][i]->lru = 0;
					return i;					
				}
			}
			// TODO: return an error code? 
			return 0;
		}	
		
		
		void update_prev_predicted_distances() {
			uint64_t size = d_table[previous_distance].size();
			int exists_index = find_distance_in_prev();
			
			if (exists_index != -1) {
				// distance already present in previous predicted, just update
				lru_update(exists_index);
			} else if (size < S) {
				// array is not full, we can just push
				p_distance *new_dist = new  p_distance(current_distance); 
				d_table[previous_distance].push_back(new_dist);
				lru_update(size); 
			} else {
				// array is full, find value to evict
				int index = lru_evict();
				lru_update(index);
			}
		}

		void update_previous_values() {
			last_accessed_page = current_page;
			previous_distance = current_distance; 
			current_page = 0;
			current_distance = 0;
		}

		std::vector<uint64_t> distances_to_addresses(std::vector<p_distance*> &distances) {
			int size = distances.size();
			if (size == 0) {
				return {};
			}
			std::vector<uint64_t> addrs;
			for (int i = 0; i < size; i++) {
				addrs.push_back(current_page + (distances[i]->distance));
			}
			return addrs;
		}

	public:
		Delta(uint64_t slots) : S(slots), 
			last_accessed_page(0),
			previous_distance(0),
			current_distance(0),
			current_page(0) {}
		
		~Delta(){}
	

		std::vector<uint64_t> find_prefetch_addrs(uint64_t full_addr) {
			set_current_values(full_addr);
			
			std::vector<p_distance*> predicted = d_table[current_distance];
			
			update_prev_predicted_distances();
			update_previous_values(); 
			
			// turn a vector of structs into a vector of addresses	
			return distances_to_addresses(predicted);
		}
};

#endif // __DELTA_H__
