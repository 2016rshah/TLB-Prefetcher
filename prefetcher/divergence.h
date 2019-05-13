#ifndef __DIV_H__
#define __DIV_H__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "champsim.h"

typedef struct correlated_page {
	uint64_t address;
	uint64_t references; 

	correlated_page(uint64_t addr) : address(addr), references(0){}
} correlated_page;

class Divergence {
	private:
		uint64_t last_accessed_page;
		uint64_t current_page;
		std::map<uint64_t, std::vector<correlated_page*> > corr_map;
		
		uint64_t get_page_addr(uint64_t full_addr) {
			uint64_t page = (full_addr >> LOG2_PAGE_SIZE) << LOG2_PAGE_SIZE;
			return page;
		}

		void set_current_values(uint64_t full_addr) {
			uint64_t page = get_page_addr(full_addr);
			last_accessed_page = current_page;
			current_page = page;
		}
		
		int find_page_in_prev() {
			int size = corr_map[last_accessed_page].size();
			for (int i = 0; i < size; i++) {
				if (corr_map[last_accessed_page][i]->address == current_page) {
					corr_map[last_accessed_page][i]->references++;
					return i; 
				}
			}
			
			return -1;
		}

		void add_page_in_prev() {
			correlated_page* new_page = new correlated_page(current_page);
			corr_map[last_accessed_page].push_back(new_page);
		}
		
	public:
		Divergence() :
			last_accessed_page(0),
			current_page(0) {}

		~Divergence(){}

		void update_corr_map(uint64_t full_addr) {
			set_current_values(full_addr);

			int exists = find_page_in_prev();

			if (exists == -1) {
				add_page_in_prev();
			}
		}	
		
		std::map<uint64_t, uint64_t> get_histogram_results() {
			std::map<uint64_t, uint64_t> result;
			
			for (auto element : corr_map) {
				int size = element.second.size();
				result[size] += 1;
			}
			
			return result;
		}

};

#endif // __DIV_H__
