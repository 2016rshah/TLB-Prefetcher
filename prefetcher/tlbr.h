#ifndef __TLBR_H__
#define __TLBR_H__

#include <bits/stdc++.h> 
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <map>
#include "champsim.h"
#include <stdexcept>

#define FOOTPRINT_SIZE (PAGE_SIZE / 64) 

typedef struct footprint {
    std::bitset<FOOTPRINT_SIZE> fp;
}footprint;

typedef struct footprint_correlation {
    footprint* fp;
    uint64_t corr_page;
footprint_correlation(footprint* f, uint64_t p) : fp(f), corr_page(p){}
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


	int get_similarity(footprint* fp1, footprint* fp2) {
		uint64_t fp1_integer;
		uint64_t fp2_integer;
		try {
			fp1_integer = fp1->fp.to_ullong();
			fp2_integer = fp2->fp.to_ullong();
		} catch (std::overflow_error& e) {
			assert("Overflow of tlbr chunking bitset");
		} 
		
		uint64_t xord = (fp1_integer ^ fp2_integer);
		std::bitset<FOOTPRINT_SIZE> xord_bitset (xord);
		return xord_bitset.count();
	}

    void update_metadata(footprint* fp_snapshot) {
		std::vector< footprint_correlation* > fpcs = metadata[prev_page];
		int max_index = 0;
		uint64_t max_similarity = 0;

		for(uint64_t i = 0; i < fpcs.size(); i++) {
	    	footprint_correlation* fpc = fpcs[i];
	    	uint64_t similarity = get_similarity(fpc->fp, fp_snapshot);
	    	if(similarity > max_similarity) {
				max_similarity = similarity;
				max_index = i;
	    	}
		}
		if(max_similarity >= fuzzing_threshold && fpcs.size() != 0) {
	    	metadata[prev_page][max_index]->corr_page = curr_page;
		} else if (maturity(fp_snapshot) >= maturity_threshold){
	    	footprint_correlation* fpc = new footprint_correlation(fp_snapshot, curr_page);
	    	metadata[prev_page].push_back(fpc);
		}

    }
	
    std::vector< uint64_t > get_correlations(footprint* fp_snapshot) {
		std::vector< footprint_correlation* > fps = metadata[curr_page];
		std::vector< uint64_t > results;
		for(uint64_t i = 0; i < fps.size(); i++) {
	    	footprint_correlation* fp = fps[i];
	    	uint64_t similarity = get_similarity(fp->fp, fp_snapshot);
	    	if(similarity >= similarity_threshold) {
				results.push_back(fp->corr_page);
	    	}
		}
		return results;
    };	

	uint64_t maturity(footprint* fp) {
		if (fp == NULL) {
			return 0;
		}
		return (uint64_t) fp->fp.count(); 
	};
    
 public:
	
	Metadata(uint64_t maturity_thresh, uint64_t sim_thresh, uint64_t fuzz_thresh):
		maturity_threshold(maturity_thresh),
    	similarity_threshold(sim_thresh),
    	fuzzing_threshold(fuzz_thresh),
    
    	prev_page(0),
    	curr_page(0) {}
	
	~Metadata(){}
    
    void tlbr_update(uint64_t addr) {
		prev_page = curr_page;
		curr_page = get_page_addr(addr);
		uint64_t curr_offset = get_offset(addr);
		// floor to nearest cache line
		curr_offset = (curr_offset >> 6);
		if (resident_footprints.count(curr_page) <= 0) {
			resident_footprints[curr_page] = new footprint();
		}
		if (	resident_footprints[curr_page]->fp[curr_offset] == 1) {
			// printf("Bit is already set\n");
		} else {
			// printf("Bit is not already set\n");
			// printf("maturity: %d\n", maturity(resident_footprints[curr_page]));
		}
		resident_footprints[curr_page]->fp[curr_offset] = 1;
		if (prev_page == 0 || curr_page == prev_page) {return;}
		footprint* fp_snapshot = resident_footprints[prev_page];
		update_metadata(fp_snapshot);
    };

    std::vector< uint64_t > tlbr_predict(uint64_t addr) {
		if (resident_footprints.count(curr_page) <= 0) {
			resident_footprints[curr_page] = new footprint();
		}
		footprint* fp_snapshot = resident_footprints[curr_page];
		
		if(maturity(fp_snapshot) < maturity_threshold) { return {}; }
		return get_correlations(fp_snapshot);
    };
	
	void remove_resident(uint64_t evicted_addr) {
		uint64_t page = get_page_addr(evicted_addr);
		resident_footprints.erase(page);
	};
};

/* Translation Lookaside BuffeR */
class TLBR {
	private:
		Metadata* pc_corr_map = new Metadata(2, 100, 5);

	public:
		TLBR() {}

		~TLBR(){}

		std::vector<uint64_t> find_prefetch_addrs(uint64_t addr, uint64_t ip) {
			pc_corr_map->tlbr_update(addr);	
			return pc_corr_map->tlbr_predict(addr);
		}
	
		void remove_resident(uint64_t evicted_addr) {
			pc_corr_map->remove_resident(evicted_addr);	
		}
		
};

#endif // __TLBR_H__
