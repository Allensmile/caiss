//
// Created by Chunel on 2020/5/23.
// 所有算法类的基类信息
//

#ifndef CAISS_ALGORITHMPROC_H
#define CAISS_ALGORITHMPROC_H


#include <string>
#include "../caissLib/CaissLib.h"
#include "../utilsCtrl/UtilsInclude.h"
#include "../threadCtrl/ThreadInclude.h"


const static unsigned int DEFAULT_STEP = 2;
const static unsigned int DEFAULT_MAX_EPOCH = 5;
const static unsigned int DEFAULT_SHOW_SPAN = 1000;    // 1000行会显示一次日志
const static std::string MODEL_SUFFIX = ".caiss";   // 默认的模型后缀


class AlgorithmProc {

public:
    explicit AlgorithmProc() {
        this->last_search_type_ = CAISS_SEARCH_DEFAULT;
        this->last_topK_ = UINT_MAX;
        this->cur_mode_ = CAISS_MODE_DEFAULT;
        getIgnoreTrie();
    }

    virtual ~AlgorithmProc() {
        CAISS_DELETE_PTR(this->ignore_trie_ptr_);
    }

    AlgorithmProc(const AlgorithmProc&) = delete;
    AlgorithmProc& operator= (const AlgorithmProc& pool) = delete;

    /**
     * 初始化状态和参数信息
     * @param mode
     * @param distanceType
     * @param dim
     * @param modelPath
     * @param func
     * @return
     */
    virtual CAISS_RET_TYPE init(CAISS_MODE mode,
                                const CAISS_DISTANCE_TYPE distanceType,
                                const unsigned int dim,
                                const char *modelPath,
                                CAISS_DIST_FUNC func) = 0;

    // train_mode
    /**
     * 开始精确训练方法
     * @param dataPath
     * @param maxDataSize
     * @param normalize
     * @param maxIndexSize
     * @param precision
     * @param fastRank
     * @param realRank
     * @param step
     * @param maxEpoch
     * @param showSpan
     * @return
     */
    virtual CAISS_RET_TYPE train(const char *dataPath, unsigned int maxDataSize, const CAISS_BOOL normalize,
                                 unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                                 unsigned int realRank, const unsigned int step=DEFAULT_STEP,
                                 unsigned int maxEpoch=DEFAULT_MAX_EPOCH,
                                 unsigned int showSpan=DEFAULT_SHOW_SPAN) = 0;

    // process_mode
    /**
     * 查询结果
     * @param info
     * @param searchType
     * @param topK
     * @param filterEditDistance
     * @param searchCBFunc
     * @param cbParams
     * @return
     */
    virtual CAISS_RET_TYPE search(void *info,
                                  CAISS_SEARCH_TYPE searchType,
                                  unsigned int topK,
                                  unsigned int filterEditDistance = 0,
                                  CAISS_SEARCH_CALLBACK searchCBFunc = nullptr,
                                  const void *cbParams = nullptr) = 0;

    /**
     * 插入结果信息
     * @param node
     * @param index
     * @param insertType
     * @return
     */
    virtual CAISS_RET_TYPE insert(CAISS_FLOAT *node, const char *index, CAISS_INSERT_TYPE insertType = CAISS_INSERT_OVERWRITE) = 0;   // label 是数据标签

    /**
     * 保存模型信息
     * @param modelPath
     * @return
     */
    virtual CAISS_RET_TYPE save(const char *modelPath = nullptr) = 0;    // 默认写成是当前模型的

    /**
     * 获取结果的长度
     * @param size
     * @return
     */
    virtual CAISS_RET_TYPE getResultSize(unsigned int& size) = 0;

    /**
     * 获取结果
     * @param result
     * @param size
     * @return
     */
    virtual CAISS_RET_TYPE getResult(char *result, unsigned int size) = 0;


    /**
     * 将某个节点放入忽略列表中，或者从忽略列表中取消
     * @param label
     * @param isIgnore 放入忽略列表/从忽略列表中取出
     * @return
     */
    virtual CAISS_RET_TYPE ignore(const char *label, CAISS_BOOL isIgnore = CAISS_TRUE) = 0;


protected:

    /**
     * 将向量归一化
     * @param node
     * @param dim
     * @return
     */
    CAISS_RET_TYPE normalizeNode(std::vector<CAISS_FLOAT>& node, unsigned int dim) {
        if (CAISS_FALSE == this->normalize_) {
            return CAISS_RET_OK;    // 如果不需要归一化，直接返回
        }

        if (dim != this->dim_) {
            return CAISS_RET_DIM;    // 忽略维度不一致的情况
        }

        CAISS_FLOAT sum = 0.0;
        for (unsigned int i = 0; i < this->dim_; i++) {
            sum += (node[i] * node[i]);
        }

        CAISS_FLOAT denominator = std::sqrt(sum);    // 分母信息
        for (unsigned int i = 0; i < this->dim_; i++) {
            node[i] = node[i] / denominator;
        }

        return CAISS_RET_OK;
    }

    /**
     * 快速求平方根
     * @param x
     * @return
     */
    float fastSqrt(float x) {
        float half = 0.5f * x;
        int i = *(int*)&x;     // get bits for floating VALUE
        i = 0x5f375a86 - (i >> 1);    // gives initial guess y0
        x = *(float*)&i;     // convert bits BACK to float
        x = x * (1.5f - half * x * x);     // Newton step, repeating increases accuracy
        return 1 / x;
    }

    /**
     * 获取挂有忽略信息的trie树
     * @return
     */
    static TrieProc* getIgnoreTrie() {
        if (nullptr == AlgorithmProc::ignore_trie_ptr_) {
            AlgorithmProc::trie_lock_.writeLock();
            if (nullptr == AlgorithmProc::ignore_trie_ptr_) {
                AlgorithmProc::ignore_trie_ptr_ = new TrieProc();
            }
            AlgorithmProc::trie_lock_.writeUnlock();
        }

        return AlgorithmProc::ignore_trie_ptr_;
    }


protected:
    std::string model_path_;
    unsigned int dim_{};
    CAISS_MODE cur_mode_;
    CAISS_BOOL normalize_{};    // 是否需要标准化数据
    std::string result_;
    ALOG_WORD2DETAILS_MAP word_details_map_;    // 记录结果使用的信息
    CAISS_DISTANCE_TYPE distance_type_;

    LruProc lru_cache_;    // 最近N次的查询记录
    unsigned int last_topK_;    // 记录上一次的topK跟这一次的topK是否相同
    CAISS_SEARCH_TYPE last_search_type_;

    static RWLock trie_lock_;
    static TrieProc* ignore_trie_ptr_;    // 标识忽略的字典树
};

#endif //CAISS_ALGORITHMPROC_H
