#ifndef __DELTA_H__
#define __DELTA_H__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "champsim.h"

typedef struct predicted_distance {
	int64_t distance;
	uint64_t lru;

	predicted_distance(uint64_t d) : distance(d), lru(0){}
}p_distance;

class Delta {
	private:
		// TODO the predicted values are supposed to be managed with LRU
		uint64_t S;
		std::unordered_map<uint64_t, std::vector<p_distance*> > d_table;
		uint64_t last_accessed_page;
		uint64_t previous_distance;
		int64_t current_distance;
		uint64_t current_page;
		int64_t max_distance = (256 << LOG2_PAGE_SIZE);

		uint64_t get_page_addr(uint64_t full_addr) {
			uint64_t page = (full_addr >> LOG2_PAGE_SIZE) << LOG2_PAGE_SIZE;
			return page;
		}

		void set_current_values(uint64_t full_addr) {
			uint64_t page = get_page_addr(full_addr);
			current_page = page;
			current_distance = (int64_t) (page - last_accessed_page);
		}

        // Helper method to see if the distance already exists in the delta table
		int find_distance_in_prev() {
			int size = d_table[previous_distance].size();
			for (int i = 0; i < size; i++) {
				if (d_table[previous_distance][i]->distance == current_distance) {
					return i;
				}
			}
			return -1;
		}

        // Update all of the LRU values, set current index to be 0 since it is MRU
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
			if (current_distance == 0 || current_distance >= max_distance) {
				return; // if the page address matches, ignore
			}
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

        // This method takes in a vector of distances and adds them to the page addresses
        // so that we can get a vector of addresses with these offsets
		std::vector<uint64_t> distances_to_addresses(std::vector<p_distance*> &distances) {
			int size = distances.size();
			if (size == 0) {
				return {};
			}
			std::vector<uint64_t> addrs;
			for (int i = 0; i < size; i++) {
				int64_t calc_distance = (distances[i]->distance);
				uint64_t positive_dist = calc_distance < 0 ? (uint64_t) (calc_distance * -1) : (uint64_t) calc_distance;
				uint64_t result = calc_distance < 0 ? current_page - positive_dist : current_page + positive_dist;
				addrs.push_back(result);
				
			}
			return addrs;
		}

	public:
		Delta(uint64_t slots) : S(slots),
			last_accessed_page(0),
			previous_distance(0),
			current_distance(0),
			current_page(0) {}

		~Delta(){ 
			for (const auto &pair : d_table) {
				for (uint64_t i = 0; i < pair.second.size(); i++) {
					delete pair.second[i];
				}
			} 
		}


		std::vector<uint64_t> find_prefetch_addrs(uint64_t full_addr) {
			set_current_values(full_addr);
			std::vector<p_distance*> predicted = d_table[current_distance];

			update_prev_predicted_distances();
			
			std::vector<uint64_t> result_addrs = distances_to_addresses(predicted);
			update_previous_values();
			// turn a vector of structs into a vector of addresses
			return result_addrs;
		}
};

#endif // __DELTA_H__
