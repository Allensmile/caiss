#!/usr/bin/env python
# encoding: utf-8

import json

import numpy as np
import tornado.ioloop
import tornado.web

from python.pyCaiss import *
from python.dataProcess.pyCaissBert import *

CAISS_LIB_PATH = r'./libCaiss.dylib'                    # caiss动态库所在路径
CAISS_MODEL_PATH = r'./demo_2500words_768dim.caiss'     # caiss模型所在路径
BERT_MODEL_PATH = r'./uncased_L-12_H-768_A-12/'          # bert模型所在路径

MAX_THREAD_SIZE = 1    # caiss最大并发数量（推荐不超过cpu核数）
DIM = 768              # 数据维度


def show_info(tnd, info):
    # 展示信息
    search_result = json.loads(info)
    result_list = search_result.get('result')
    for result in result_list:
        tnd.write('The query info is : [{0}], '.format(result.get('query')))

        result_words = []
        for detail in result.get('details'):
            result_words.append(detail.get('label'))
        tnd.write('and the info you may also want to query maybe : {}.'.format(result_words))
        tnd.write('<br>')


class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Hello, welcome to the world of Caiss.")


class CaissWordHandler(tornado.web.RequestHandler):
    def get(self):
        query_word = self.get_argument('query', '')
        if len(query_word) == 0:
            self.write('please enter query word.')
            return
        top_k = self.get_argument('top', '5')

        ret, result_str = caiss.sync_search(handle, query_word, CAISS_SEARCH_WORD, int(top_k), 0)
        if 0 != ret:
            self.write('search failed for the reason of : [' + ret + ']')
            return

        show_info(self, result_str)


class CaissSentenceHandler(tornado.web.RequestHandler):
    def get(self):
        query_sent = self.get_argument('sent', '')
        if len(query_sent) == 0:
            self.write('please enter sentence info.')
            return

        if query_sent[0].isalnum() is False:
            self.write('please enter english sentence.')
            return

        indices, segments = tokenizer.encode(first=query_sent, max_len=200)    # 句子最长200个单词
        # 在词向量训练任务中，固定获取第一个词语的信息
        res_vec = bert_model.predict([np.array([indices]), np.array([segments])])[0][0].tolist()

        top_k = self.get_argument('top', '3')
        ret, result_str = caiss.sync_search(handle, res_vec, CAISS_SEARCH_QUERY, int(top_k), 0)
        if 0 != ret:
            self.write('search failed for the reason of : ' + ret)
            return

        show_info(self, result_str)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/caiss/word", CaissWordHandler),
        (r'/caiss/sentence', CaissSentenceHandler)
    ])


def tornado_server_start():
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()


if __name__ == "__main__":
    # http://127.0.0.1:8888/caiss/word?query=water
    tokenizer = build_bert_tokenizer(BERT_MODEL_PATH)

    bert_model = build_bert_model(BERT_MODEL_PATH)

    caiss = PyCaiss(CAISS_LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)
    handle = c_void_p(0)
    caiss.create_handle(handle)
    caiss.init(handle, CAISS_MODE_PROCESS, CAISS_DISTANCE_INNER, DIM, CAISS_MODEL_PATH)
    print('[caiss] environment init success...')

    tornado_server_start()    # 开启tornado服务，对外提供能力

    caiss.destroy(handle)
