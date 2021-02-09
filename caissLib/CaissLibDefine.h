/******************************
 * @Author Chunel
 * @Name CaissLibDefine.h
 * @Date 2020/5/15 1:23 上午
 * @Desc Caiss库类型定义头文件
 * @Version 3.0.1
 ******************************/

#ifndef _CAISS_LIBRARY_DEFINE_H_
#define _CAISS_LIBRARY_DEFINE_H_

#ifdef _WIN32
    #define STDCALL __stdcall
    #ifndef CAISS_LIB_HEAD
        #if defined(_CAISS_EXPORTS_)
            #define CAISS_LIB_HEAD __declspec(dllexport)
        #else
            #define CAISS_LIB_HEAD __declspec(dllimport)
        #endif
    #endif
#else
    #define STDCALL
    #define CAISS_LIB_HEAD
#endif


#include <vector>
#include <list>
#include <string>

/* 数据类型定义 */
using CAISS_STATUS = int;
using CAISS_HANDLE = void*;
using CAISS_VOID = void;
using CAISS_UINT = unsigned int;
using CAISS_FLOAT = float;
using CAISS_BOOL = int;
using CAISS_STRING = const char*;

using CAISS_FLOAT_ARRAY = std::vector<CAISS_FLOAT>;
using CAISS_STRING_ARRAY = std::vector<std::string>;

/* 自定义用于计算距离的函数 */
typedef CAISS_FLOAT (STDCALL *CAISS_DIST_FUNC)(CAISS_VOID *vec1, CAISS_VOID *vec2, const CAISS_VOID *params);
/* 查询到结果后，触发的回调函数 */
typedef CAISS_VOID (STDCALL *CAISS_SEARCH_CALLBACK)(const char *query, const CAISS_STRING_ARRAY &infos, const CAISS_FLOAT_ARRAY &distances, const CAISS_VOID *params);

/* 定义函数返回类型 */
#define CAISS_API    CAISS_LIB_HEAD CAISS_STATUS STDCALL

/* 函数返回值定义 */
#define CAISS_RET_WARNING       (1)     // 流程告警
#define CAISS_RET_OK            (0)     // 流程正常
#define CAISS_RET_ERR           (-1)    // 流程异常
#define CAISS_RET_RES           (-2)    // 资源问题
#define CAISS_RET_MODE          (-3)    // 模式选择问题
#define CAISS_RET_PATH          (-4)    // 路径问题
#define CAISS_RET_JSON          (-5)    // json解析问题
#define CAISS_RET_PARAM         (-6)    // 参数问题
#define CAISS_RET_HANDLE        (-7)    // 句柄申请问题
#define CAISS_RET_DIM           (-8)    // 维度问题
#define CAISS_RET_MODEL_SIZE    (-9)    // 模型尺寸限制问题
#define CAISS_RET_WORD_SIZE     (-10)   // 词语长度限制问题
#define CAISS_RET_SQL_PARSE     (-11)   // 传入的sql解析问题
#define CAISS_RET_MODEL_DATA    (-12)   // 模型内容问题
#define CAISS_RET_RESULT_SIZE   (-13)   // 结果长度问题
#define CAISS_RET_NO_SUPPORT    (-99)   // 暂不支持该功能


#define CAISS_TRUE       (1)
#define CAISS_FALSE      (0)


enum CAISS_MODE {
    CAISS_MODE_DEFAULT = 0,    // 无效模式
    CAISS_MODE_TRAIN = 1,      // 训练模式
    CAISS_MODE_PROCESS = 2,    // 处理模式
};

enum CAISS_SEARCH_TYPE {
    CAISS_SEARCH_DEFAULT = 0,
    CAISS_SEARCH_QUERY = 1,    // 通过快速检索的方式，查询query信息
    CAISS_SEARCH_WORD = 2,     // 通过快速检索的方式，查询word信息
    CAISS_LOOP_QUERY = 3,      // 通过暴力循环的方式，查询query信息
    CAISS_LOOP_WORD = 4        // 通过暴力循环的方式，查询word信息
};

enum CAISS_INSERT_TYPE {
    // 如果插入相同的数据
    CAISS_INSERT_DEFAULT = 1,
    CAISS_INSERT_OVERWRITE = 1,   // 覆盖模型中原有的数据
    CAISS_INSERT_DISCARD = 2,     // 丢弃当前插入的数据
};

enum CAISS_MANAGE_TYPE {
    CAISS_MANAGE_DEFAULT = 1,
    CAISS_MANAGE_SYNC = 1,     // 同步控制
    CAISS_MANAGE_ASYNC = 2,    // 异步控制
};

enum CAISS_DISTANCE_TYPE {
    CAISS_DISTANCE_DEFAULT = 1,
    CAISS_DISTANCE_EUC = 1,         // 欧氏距离
    CAISS_DISTANCE_INNER = 2,       // 内积距离
    CAISS_DISTANCE_JACCARD = 3,     // 杰卡德距离
    CAISS_DISTANCE_EDITION = 99,    // 自定义距离（注：设定自定义距离时，必须是较小的值，表示较为接近）
};

enum CAISS_ALGO_TYPE {
    CAISS_ALGO_DEFAULT = 1,
    CAISS_ALGO_HNSW = 1,            // hnsw算法（推荐）
    CAISS_ALGO_MRPT = 2             // mrpt算法
};


const static int CAISS_MIN_EDIT_DISTANCE = -1;    // 不根据编辑距离过滤
const static int CAISS_DEFAULT_EDIT_DISTANCE = 0;    // 仅过滤编辑距离为0的词语（相同词语）
const static int CAISS_MAX_EDIT_DISTANCE = 5;    // 最大编辑距离（超过则返回CAISS_RET_PARAM）


#endif //_CAISS_LIBRARY_DEFINE_H_
