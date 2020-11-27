#ifndef WINDOW_SKETCH_BENCHMARK_H
#define WINDOW_SKETCH_BENCHMARK_H

#include <vector>

//#include "RHHH.h"
#include "Univmon.h"
#include "Elastic.h"
//#include "BeauCoup.h"
#include "SpaceSaving.h"

#include "Ours.h"
#include "CUOurs.h"
#include "SimpleOurs.h"

#include "SketchMerge.h"
//#include "TrafficMerge.h"

#include "MMap.h"
#include "Timer.h"


template<typename DATA_TYPE,typename COUNT_TYPE>
class BenchMark{
public:

    typedef Abstract<DATA_TYPE, COUNT_TYPE>* Sketch;
    typedef std::vector<Sketch> SketchVector;
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    BenchMark(const char* PATH){
        result = Load(PATH);
        dataset = (DATA_TYPE*)result.start;
        length = result.length / sizeof(DATA_TYPE);

        for(uint64_t i = 0;i < length;++i){
            mp[dataset[i]] += 1;
        }
    }

    ~BenchMark(){
        UnLoad(result);
    }

    void FEBench(){
        SketchVector sketches = {
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(1, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(2, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(4, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(8, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(16, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(32, 5000000),
/*
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CSketch<DATA_TYPE, COUNT_TYPE>>(1, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CSketch<DATA_TYPE, COUNT_TYPE>>(2, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CSketch<DATA_TYPE, COUNT_TYPE>>(4, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CSketch<DATA_TYPE, COUNT_TYPE>>(8, 5000000),
*/
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(1, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(2, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(4, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(8, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(16, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(32, 5000000),
       };
        
        uint32_t count = 0;
        for(auto sketch : sketches){
            std::cout << "K=" << pow(2,count) << std::endl;
            SketchThp(sketch);
            FECheckError(sketch);
            delete sketch;
           count++;
           if(count == 6){count=0;}
        }
    }

    void HHBench(double alpha){
        SketchVector sketches = {
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(1, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(2, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(4, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(8, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(16, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, Elastic<DATA_TYPE, COUNT_TYPE>>(32, 5000000),
/*
                new SketchMerge<DATA_TYPE, COUNT_TYPE, UnivMon<DATA_TYPE, COUNT_TYPE>>(1, 250000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, UnivMon<DATA_TYPE, COUNT_TYPE>>(2, 250000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, UnivMon<DATA_TYPE, COUNT_TYPE>>(4, 250000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, UnivMon<DATA_TYPE, COUNT_TYPE>>(8, 250000),
*/
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(1, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(2, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(4, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(8, 5000000),
                new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(16, 5000000),
	new SketchMerge<DATA_TYPE, COUNT_TYPE, CUOurs<DATA_TYPE, COUNT_TYPE>>(32, 5000000),

        };
	
        uint32_t count = 0;
        for(auto sketch : sketches){
            std::cout << "K=" << pow(2,count) << std::endl;
            SketchThp(sketch);
            HHCheckError(sketch, alpha * length);
            delete sketch;
            count++;
            if(count == 6){count=0;}
        }
    }

private:
    LoadResult result;

    DATA_TYPE* dataset;
    uint64_t length;

    HashMap mp;


    void SketchThp(Sketch sketch){
        TP start, finish;

        start = now();
        sketch->Insert(dataset, length);
        finish = now();

        //cout << sketch->name << endl;
        //cout << "Thp: " << length / durationms(finish, start) << endl;
    }

    void FECheckError(Sketch sketch){
        double aae = 0, are = 0;

        for(auto it = mp.begin();it != mp.end();++it){
            COUNT_TYPE estimated = sketch->Query(it->first);
            aae += abs(it->second - estimated);
            are += abs(it->second - estimated) / (double)it->second;
        }

        cout << sketch->name << endl;
        cout << "AAE: " << aae / mp.size() << endl;
        cout << "ARE: " << are / mp.size() << endl;
    }

    void HHCheckError(Sketch sketch, uint32_t thres){
        double aae = 0, are = 0, both = 0, hh = 0;

        HashMap ret = sketch->HHQuery(thres);

        for(auto it = mp.begin();it != mp.end();++it){
            if(it->second > thres){
                hh += 1;
                if(ret.find(it->first) != ret.end()){
                    both += 1;
                    COUNT_TYPE estimated = ret[it->first];
                    aae += abs(it->second - estimated);
                    are += abs(it->second - estimated) / (double)it->second;
                }
            }
        }

        cout << sketch->name << endl;
        cout << "AAE: " << aae / both << endl;
        cout << "ARE: " << are / both << endl;
        cout << "CR: " << both / hh << endl;
        cout << "PR: " << both / ret.size() << endl;
    }


};

#endif //WINDOW_SKETCH_BENCHMARK_H
