"""
direct to book
"""

import sys
import os
import StringIO
import EXIF
from PIL import Image


import cairo

import xml.sax as sax
import time
import copy

from decimaldegrees import dms2decimal


GPS_tags = ('GPS GPSAltitude', 'GPS GPSAltitudeRef', 'GPS GPSLatitude', 'GPS GPSLatitudeRef', 'GPS GPSLongitude', 'GPS GPSLongitudeRef', 'GPS GPSTimeStamp',)
GPS_LAT_K = GPS_tags[2]
GPS_LON_K = GPS_tags[4]


class GPXHandler(sax.ContentHandler):
    def __init__(self):
        self.points = []
        self.cur_point_ = {}
        self.start_time_ = False
    def startElement(self, name, attrs):
        if name == 'trkpt':
            self.cur_point_['lat'] = attrs.get('lat')
            self.cur_point_['lon'] = attrs.get('lon')
        elif name == 'time':
            self.start_time_ = True
            
    def endElement(self,name):
        if name == 'trkpt':
            self.points.append(copy.copy(self.cur_point_))
        if name == 'time':
            self.cur_point_['time'] = ''
            self.start_time_ = False
            
    def characters(self, content):
        if self.start_time_:
            if 'time' in self.cur_point_:
                self.cur_point_['time'] = self.cur_point_['time'] + content
            else:
                self.cur_point_['time'] = content
            
class ImagesGPSParser(object):
    def __init__(self, dirpath):
        self.images = []
        entries = os.listdir(dirpath)
        for d in entries:
            path = os.path.join(dirpath,d)
            if os.path.isdir(path):
                continue
            file=open(path, 'rb')
            data = EXIF.process_file(file, stop_tag='UNDEF', details=True)
            if not data:
                continue
            lat = data[GPS_LAT_K].values
            lon = data[GPS_LON_K].values
            self.images.append({'path':path, 'lat':self.dms_to_dd(lat), 'lon':self.dms_to_dd(lon)})
            
    def dms_to_dd(self, dms):
        d = dms[0].num  / dms[0].den
        m = dms[1].num  / dms[1].den
        s = dms[2].num  / dms[2].den
        return dms2decimal(d,m,s)
            
        
class PDFComposer(object):
    def __init__(self, images):
        self.images = images
        min_y = 999999
        max_y = -999999
        for i in images:
            min_y = min(min_y, i['lon'])
            max_y = max(min_y, i['lon'])
            
        h = max_y - min_y
        
        self.get_context(400,h)
        for i in images:
            self.insert_image(i['path'], 22, i['lon'] - min_y)
        
    def get_context(self, w, h):
        self.surface = cairo.PDFSurface('foo.pdf',w,h)
        self.context = cairo.Context(self.surface)
        
    def insert_image(self, path, x,y):
        im = Image.open(path)
        im.load()
        data = im.getdata()
        s = cairo.ImageSurface(cairo.FORMAT_ARGB32, im)
        


if __name__ == '__main__':
    parser = sax.make_parser()
    h = GPXHandler()
    parser.setContentHandler(h)
    parser.parse(open(sys.argv[1],"r"))
    count = 1
    #for p in h.points:
        #print('%d: %s %s'%(count,p['lat'], p['lon'])) 
        #count += 1
        
    #count = 1
    #ip = ImagesGPSParser(sys.argv[2])
    #for p in ip.images:
        #print('%d: %s %s [%s]'%(count,p['lat'], p['lon'],p['path'])) 
        #count += 1
        
    