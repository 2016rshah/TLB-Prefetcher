#ifndef __PC_H_H__
#define __PC_H_H__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <map>
#include "champsim.h"
#include <queue>

typedef struct delta_entry {
	int64_t delta;
	uint64_t lru;

	delta_entry(int64_t d) : delta(d), lru(0){}
}delta_entry;

class correlation_state {
	private:
		uint64_t S;
		uint64_t current_page;
		uint64_t last_page;
		int64_t current_delta;
		uint64_t current_offset;
		
		std::queue<uint64_t> history; 
		std::map<std::queue<uint64_t>, std::vector<delta_entry*> > correlation;

		
		uint64_t get_page_addr(uint64_t full_addr) {
			uint64_t page = (full_addr >> LOG2_PAGE_SIZE) << LOG2_PAGE_SIZE;
			return page;
		}
		uint64_t get_offset(uint64_t full_addr) {
			return ~(-1 << LOG2_PAGE_SIZE) & full_addr;
		}

		void set_current_values(uint64_t full_addr) {
			uint64_t page = get_page_addr(full_addr);
			last_page = current_page;
			current_page = page;
			current_delta = (current_page - last_page);
			current_offset = get_offset(full_addr);
		}

		void update_history_queue() {
			history.pop();
			history.push(current_offset);
		}
		
		int find_delta_in_prev() {
			int size = correlation[history].size();
			for (int i = 0; i < size; i++) {
				if (correlation[history][i]->delta == current_delta) {
					return i;
				}
			}
			return -1;
		}

		void lru_update(int index) {
			int max = correlation[history].size();
			for (int i = 0; i < max; i++) {
				correlation[history][i]->lru++;
			}
			correlation[history][index]->lru = 0;
		}

		// only call when lru is full
		int lru_evict() {
			for (uint64_t i = 0; i < S; i++) {
				if (correlation[history][i]->lru == S - 1) {
					// we've found a value to evict, replace with current
					correlation[history][i]->delta = current_delta;
					correlation[history][i]->lru = 0;
					return i;
				}
			}
			return 0;
		}

		void update_prev_correlation() {
			if (current_page == last_page) {
				return; // if the page address matches, ignore
			}
			uint64_t size = correlation[history].size();
			int exists_index = find_delta_in_prev();

			if (exists_index != -1) {
				// distance already present in previous predicted, just update
				lru_update(exists_index);
			} else if (size < S) {
				// array is not full, we can just push
				delta_entry *new_entry = new  delta_entry(current_delta);
				correlation[history].push_back(new_entry);
				lru_update(size);
			} else {
				// array is full, find value to evict
				int index = lru_evict();
				lru_update(index);
			}
		}

		std::vector<uint64_t> get_predictions() {
			if (correlation.count(history) == 0) {return {};}
			
			std::vector<delta_entry*> deltas = correlation[history];

			std::vector<uint64_t> results;
			for(int i =0; i < deltas.size(); i++) {
				int64_t d = deltas[i]->delta;
				if (d < 0 && ((current_page - d) > 0)) {
					d *= -1; 
					results.push_back(current_page - d);
					continue;
				}
				results.push_back(current_page + d);
			}
			return results;
		}

	public:
		correlation_state(uint64_t slots) :
			S(slots),
			current_page(0),
			last_page(0) {}

		~correlation_state(){}


		std::vector<uint64_t> find_prefetch_addrs(uint64_t full_addr) {
			set_current_values(full_addr);
			if (history.size() < 5) {
				history.push(current_offset);
				return{};
			}
			update_prev_correlation();
			update_history_queue();
			
			return get_predictions();
		}

};

/* PC-Localized Address Correlation */
class PC_HISTORY {
	private:
		uint64_t S;
		std::map<uint64_t, correlation_state*> pc_corr_map;

	public:
		PC_HISTORY(uint64_t slots) : S(slots){}

		~PC_HISTORY(){}

		std::vector<uint64_t> find_prefetch_addrs(uint64_t addr, uint64_t ip) {
			if (ip == 0) {
				// dummy ip
				return {};
			}
			if (pc_corr_map.count(ip) == 0) {
				correlation_state* new_state = new correlation_state(S);
				pc_corr_map[ip] = new_state;
			}	
			return pc_corr_map[ip]->find_prefetch_addrs(addr);
		}
		
};

#endif // __PC_H_H__
