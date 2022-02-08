#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from urllib import parse
from flask import Flask
from flask import Blueprint,render_template, request, redirect, make_response

import json
import os

from tornado.wsgi import WSGIContainer
from tornado.httpserver import HTTPServer
from tornado.ioloop import IOLoop

import requests
import inspect

from urllib import parse
from urllib.parse import urlparse

app = Flask(__name__)

g_hostdict = {}
g_wordsdict = {}

def getcurpath():
    return os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

def transviddata( wording, srcdata, savepath):
    ffmpeg = '{}/ffmpeg'.format(getcurpath())
    savepath = '/data/xxxxx' + savepath
    input = savepath + '.in.ts'
    output = savepath + '.out.ts'
    print(savepath)
    try:
        with open(input, 'wb') as f:
            f.write(srcdata)
        
        print(' -------- write file ok ---------')
        cmd = "{} -i '{}' -vf drawtext=fontfile=Arial.ttf:fontcolor=black:fontsize=50:text='{}':x=0:y=100 -copyts -muxdelay 0 -vsync 0 -acodec copy -y '{}'".format(ffmpeg, 
            input, wording, output)

        print(cmd)

        os.system(cmd)
        
        
        if os.path.isfile(output):
            data = open(output, 'rb').read()
            return data
    except Exception as e:
        print(e)
        raise
    finally:
        if os.path.isfile(input):
            os.remove(input)
        if os.path.isfile(savepath):
            os.remove(savepath)
        pass

    return None



@app.route('/')
def index():
    return 'index'

@app.route('/src')
def test():
    return 'src'

@app.route('/play',methods=['GET'])
def play():
    '''
    /play?url=xx
    '''
    print( 'play req info, baseurl: {}, args: {}, headers: {}'.format(request.base_url, str(request.args), str(request.headers)))
    if request.method == 'GET':
        url = request.args.get('url')
        words = request.args.get('words')
        if words is None:
            words = 'test_test_test'
        psret = urlparse(url)
        print('host: {}, path : {}'.format(psret.hostname, psret.path))
        k = '.'.join(psret.path.split('/')[-1].split('.')[0:1])
        g_hostdict[k] = '/'.join(url.split('/')[0:-1])
        g_wordsdict[k] = words
        print(g_hostdict)
        rsp = requests.get(url)
        if rsp.status_code == 200:
            ret = make_response(rsp.content)
            ret.headers['Content-Type'] = 'application/x-mpegURL'
            return ret

    return 'Unknown Request'

#http://9.21.132.173:8080/play?words=testsets&url=http://defaultts.tc.qq.com/defaultts.tc.qq.com/VFafrMHc30VA4zTcPv25N6oBYXwyOotDh_QT5XoJdyjgxId3y9oLB1XW84sd59bUgj69HSxn6V8YYv2N5s1gI_fwttUEn8d_ocTeLUAxzbSep894qRDbJg/v00342m0ww8.321001.ts.m3u8?ver=4
@app.errorhandler(404)
def eh_(err):
    vk = '.'.join(request.path.split('.')[0:1])
    vk = vk.split('_')[-1]
    if vk in g_hostdict:
        host = g_hostdict[vk]
        newurl = '{}{}'.format(host, request.full_path)
        print('new url: {}'.format(newurl))
        headers = {}
        #headers['X-Playback-Session-Id'] = request.headers.get('X-Playback-Session-Id')
        rsp = requests.get(newurl, headers = headers)
        if rsp.status_code == 200:
            words = g_wordsdict[vk]
            data = transviddata(words, rsp.content, request.path)
            ret = make_response(data)
            ret.headers['Content-Type'] = 'video/MP2T'
            return ret
        else:
            print('{}'.format(rsp.text))
            return rsp.content.decode('utf8')
        
    return "test"

def run_as_flask():
    app.run('0.0.0.0', 8080)

def run_as_tornado():
    http_server = HTTPServer(WSGIContainer(app))
    http_server.listen(8080)
    IOLoop.instance().start()

if __name__ == '__main__':
    #run_as_flask()

    # 采用tornado实现高性能
    run_as_tornado()