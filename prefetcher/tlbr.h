#ifndef __TLBR_H__
#define __TLBR_H__

#include <bits/stdc++.h> 
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <map>
#include "champsim.h"



typedef struct footprint {
    bitset<PAGE_SIZE> fp;
}footprint;

typedef struct footprint_correlation {
    footprint fp;
    uint64_t corr_page;
footprint_correlation(footprint f, uint64_t p) : fp(f), corr_page(p){}
}footprint_corr;

class Metadata {
 private:
    uint64_t maturity_threshold;
    uint64_t similarity_threshold;
    uint64_t fuzzing_threshold;
    
    std::map< uint64_t, std::vector< footprint_correlation* > > metadata;
    std::map< uint64_t, footprint* > resident_footprints;
    uint64_t prev_page;
    uint64_t curr_page;

    uint64_t get_page_addr(uint64_t full_addr) {
	uint64_t page = (full_addr >> LOG2_PAGE_SIZE) << LOG2_PAGE_SIZE;
	return page;
    };

    uint64_t get_offset(uint64_t full_addr) {
	return (~(-1 << LOG2_PAGE_SIZE) & full_addr);
    };

    void update_metadata(footprint* fp_snapshot) {
	std::vector< footprint_correlation* > fpcs = metadata[prev_page];
	int max_index = 0;
	uint64_t max_similarity = 0;
	for(int i = 0; i < fpcs.size(); i++) {
	    footprint_correlation* fpc = fpcs[i];
	    uint64_t similarity = get_similarity(fpc->fp, fp_snapshot);
	    if(similarity > max_similarity) {
		max_similarity = similarity;
		max_index = i;
	    }
	}
	if(max_similarity > fuzzing_threshold) {
	    metadata[prev_page][max_index] = curr_page;
	}
	else {
	    footprint_correlation* fpc = new footprint_correlation(*fp_snapshot, curr_page);
	    
	    metadata[prev_page].push_back(fpc);
	}
    }
    
    std::vector< uint64_t > get_correlations(footprint* fp_snapshot) {
	std::vector< footprint_correlations* > fps = get_metadata(curr_page);
	std::vector< uint64_t > results;
	for(int i = 0; i < fps.size(); i++) {
	    footprint_correlation* fp = fps[i];
	    uint64_t similarity = get_similarity(fp->fp, fp_snapshot);
	    if(similarity >= similarity_threshold) {
		results.push(fp->corr_page);
	    }
	}
	return results;
    };
    
 public:
    
    void tlbr_update(uint64_t addr) {
	prev_page = curr_page;
	curr_page = get_page_addr(addr);
	uint64_t curr_offset = get_offset(addr);
	resident_footproints[curr_page][curr_offset] = 1;
	footprint* fp_snapshot = resident_footprints[prev_page];
	if(maturity(fp_snapshot) < maturity_threshold) { return; }

	update_metadata(fp_snapshot);
    };

    std::vector< uint64_t > tlbr_predict(uint64_t addr) {
	footprint* fp_snapshot = resident_footprints[curr_page];
	if(maturity(fp_snapshot) < maturity_threshold) { return; }
	return get_correlations(fp_snapshot);
    };
};

/* Translation Lookaside BuffeR */
class TLBR {
	private:
		uint64_t S;
		std::map<uint64_t, correlation_state*> pc_corr_map;

	public:
		TLBR(uint64_t slots) : S(slots){}

		~TLBR(){}

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

#endif // __TLBR_H__
