"""
direct to book
"""

import sys
import os
import array
import StringIO
import EXIF
from PIL import Image


import cairo

import xml.sax as sax
import time
import copy

from decimaldegrees import dms2decimal

from dateutil import parser as dateparser

import csv

GPS_tags = ('GPS GPSAltitude', 'GPS GPSAltitudeRef', 'GPS GPSLatitude', 'GPS GPSLatitudeRef', 'GPS GPSLongitude', 'GPS GPSLongitudeRef', 'GPS GPSTimeStamp',)
GPS_LAT_K = GPS_tags[2]
GPS_LON_K = GPS_tags[4]

PAGE_HEIGHT = 1000


class GPXHandler(sax.ContentHandler):
    def __init__(self):
        self.points = []
        self.cur_point_ = {}
        self.start_time_ = False
        self.time_buffer = ''
    def startElement(self, name, attrs):
        if name == 'trkpt':
            self.cur_point_['lat'] = float(attrs.get('lat'))
            self.cur_point_['lon'] = float(attrs.get('lon'))
        elif name == 'time':
            self.start_time_ = True
            self.time_buffer = ''
            
    def endElement(self,name):
        if name == 'trkpt':
            self.points.append(copy.copy(self.cur_point_))
        if name == 'time':
            self.cur_point_['time'] = dateparser.parse(self.time_buffer, ignoretz=True)
            self.start_time_ = False
            
    def characters(self, content):
        if self.start_time_:
            self.time_buffer = self.time_buffer + content
            
class Positioner(object):
    def __init__(self, points):
        self.pts = points
        self.pts.sort(key=lambda k: k['time'])
        
    def find_interval(self, t, start, end):
        if end - start <= 2:
            return (self.pts[start], self.pts[end-1])
        
        m = int(start + ((end - start) / 2))
        #print('%s %s %s %s'%(start, end, ((end - start) / 2), m))
        mt = self.pts[m]['time']
        if mt > t:
            return self.find_interval(t, start, m)
        return self.find_interval(t, m, end)
        
    def get_pos(self, t):
        """
        Interpolate position based on given points array
        @t a datetime object
        """
        start, end = self.find_interval(t, 0, len(self.pts))
        
        delta_p =  end['time'] - start['time']
        delta_t = t - start['time']
        interpolate_f = 0
        if delta_p.total_seconds() > 0:
            interpolate_f = delta_t.total_seconds() / delta_p.total_seconds()
        lat = start['lat'] + ((end['lat'] - start['lat']) * interpolate_f)
        #print('%s %s'%(start['time'], end['time']-start['time']))
        #print(' :: '.join(['P',t.isoformat(), str(lat)]))
        return {'lat': lat, 'lon':start['lon']}
            
            
class ImagesGPSParser(object):
    def __init__(self, dirpath, scale=1.0):
        self.scale = scale
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
            #print('%s %f %f'%(d,self.dms_to_dd(lat),self.dms_to_dd(lon)))
            self.images.append({'type':'image', 'path':path, 'lat':self.dms_to_dd(lat), 'lon':self.dms_to_dd(lon)})
        self.images.sort(key=lambda i: i['lat'])
            
    def dms_to_dd(self, dms):
        d = dms[0].num  / dms[0].den
        m = dms[1].num  / dms[1].den
        s = dms[2].num  / dms[2].den
        return dms2decimal(d,m,s) * self.scale
        
        
        
        
class PDFComposerCairo(object):
    def __init__(self, objects, scale=1.0):
        self.extrema = [0,0]
        self.objects = objects
        min_y = 999999.0
        max_y = -999999.0
        for obj in objects:
            min_y = min(min_y, obj['lat'] * scale)
            max_y = max(min_y, obj['lat'] * scale)
            
        h = max_y - min_y
        self.num_page = 0
        
        self.previous_image_ = None
        self.get_context(400, PAGE_HEIGHT)
        for obj in objects:
            insert = getattr(self, '_'.join(['insert', obj['type']]))
            insert(obj, 0, (obj['lat'] * scale) - (min_y + (self.num_page * PAGE_HEIGHT)))
            #self.insert(i['path'], 0, (i['lat'] * scale) - (min_y + (self.num_page * PAGE_HEIGHT)))
        self.surface.finish()
        
    def insert_text(self, o, x, y):
        self.context.save()
        self.context.move_to(x, y)
        self.context.show_text(o['msg'])
        self.context.restore()
        
    def get_context(self, w, h):
        self.surface = cairo.PDFSurface('foo.pdf',w,h)
        self.context = cairo.Context(self.surface)
        
        
    def hi_res_(self):
        scale = 300.0/72.0;
        pat = self.context.get_source()
        mat = pat.get_matrix()
        xx, yx, xy, yy, x0, y0 = mat
        mat.translate(0,0)
        mat.scale(scale,scale)
        mat.translate(x0 , y0)
        pat.set_matrix(mat)
    
    def weave_image(self, im, x, y):
        if not self.previous_image_:
            return False
        if self.num_page != self.previous_image_['p']:
            return False
        pb = self.previous_image_['y'] + self.previous_image_['h']
        if y > pb:
            return False
        
        tw = int(min(im.size[0], self.previous_image_['w']))
        th = int(pb - y)
        
        tim = Image.new('RGBA', (tw,th))
        
        pim = self.previous_image_['i']
        iy = int(y)
        p_offset = iy - self.previous_image_['y']
        a = True
        pp = tim.im.putpixel
        try:
            for ty in xrange(th):
                a = not a
                for tx in xrange(tw):
                    if a:
                        a = False
                        pp((tx,ty) , pim.getpixel((tx,ty + p_offset)))
                    else:
                        a = True
                        pp((tx,ty) , im.getpixel((tx,ty)))
        except IndexError:
            #print('ERROR: Failed to weave')
            return False
            
        try:            
            data = array.array('B', tim.tostring('raw', 'BGRA'))
            stride = cairo.ImageSurface.format_stride_for_width(cairo.FORMAT_ARGB32, tw)
            s = cairo.ImageSurface.create_for_data(data, cairo.FORMAT_ARGB32, tw, th, stride)
        except Exception:
            return False
        self.context.save()
        
        adj_ = 72.0 / 300.0
        TX = (im.size[0] * adj_)
        if y < self.extrema[1]:
            TX = self.extrema[0]
        self.extrema[0] = TX + (tw * adj_)
        self.extrema[1] = y + th
        
        #print('OK: weaved successfully %s @ %d + %d'%(tim.size, TX, y))
        
        self.context.set_source_surface(s, TX, y)
        self.hi_res_()
        self.context.paint()
        self.context.restore()
        
        return True
        
    def insert_image(self, o, x,y):
        path = o['path']
        if y > PAGE_HEIGHT:
            y = 0
            self.num_page += 1
            self.context.show_page()
        im = Image.open(path)
        im.load()
        im.putalpha(256)
        idata = im.convert('RGBA').tostring('raw', 'BGRA')
        #print('typeof idata[0] => %s'%type(idata[0]))
        data = array.array('B', idata)
        iw, ih = im.size
        #print('[%s](%s) /%d  %d x %d @ %f + %f'%(path,im.mode,self.num_page,iw,ih,x,y))
        stride = cairo.ImageSurface.format_stride_for_width(cairo.FORMAT_ARGB32, iw)
        s = cairo.ImageSurface.create_for_data(data, cairo.FORMAT_ARGB32, iw, ih, stride)
        if not self.weave_image(im, x,y):
            self.context.save()
            self.context.set_source_surface(s, x, y)
            self.hi_res_()
            self.context.paint()
            self.context.restore()
        
        self.previous_image_ = {'i':im.copy(), 'x':x, 'y':y, 'h':ih, 'w':iw, 'p':self.num_page}
     
     
     
     
class PDFComposerScribus(object):
    def __init__(self, images, scale=1):
        #self.scribus = __import__('scribus') 
        self.images = images
        min_y = 999999
        max_y = -999999
        for i in images:
            min_y = min(min_y, i['lat'] * scale)
            max_y = max(min_y, i['lat'] * scale)
            
        h = max_y - min_y
        for i in images:
            self.insert_image(i['path'], 22, (i['lat'] * scale) - min_y)
        
    def insert_image(self, path, x,y):
        im = Image.open(path)
        #im.load()
        iw, ih = im.size
        iname = scribus.createImage(x, y, iw, ih)
        scribus.messagebarText(u'Insert Image [%s] @ %f'%(path,y))
        scribus.loadImage(path, iname)
    
class SMSReader:
    def __init__(self, fn):
        self.texts = []
        with open(fn, 'rb') as csvfile:
            smsreader = csv.reader(csvfile, delimiter=',', quotechar='"')
            for r in smsreader:
                sstring = ' '.join(r[:2])
                stime = dateparser.parse(sstring)
                #print(stime.isoformat())
                sms = {'type':'text', 'time':stime, 'from':r[2], 'to':r[3], 'msg':r[4]}
                self.texts.append(sms)
        

def main():
    scale = 2000;
    #print(tr.texts)
    parser = sax.make_parser()
    h = GPXHandler()
    parser.setContentHandler(h)
    parser.parse(open(sys.argv[1],"r"))
    pr = Positioner(h.points)
    
    ip = ImagesGPSParser(sys.argv[2], scale)
    
    objects = ip.images
    
    tr = SMSReader(sys.argv[3])
    for t in tr.texts:
        coord = pr.get_pos(t['time'])
        tobj = t
        t['lat'] = coord['lat'] * scale
        t['lon'] = coord['lon'] * scale
        objects.append(tobj)
     
    objects.sort(key=lambda k: k['lat'])
    comp = PDFComposerCairo(objects)
    
    #count = 1
    #h.points.sort(key=lambda k: k['time'])
    #for p in h.points:
        #print('%d: %s %s %s'%(count,p['lat'], p['lon'], p['time'])) 
        #count += 1
        
    #count = 1
    
    #for p in ip.images:
        #print('%d: %s %s [%s]'%(count,p['lat'], p['lon'],p['path'])) 
        #count += 1
        
try:
        import scribus
        idir = scribus.fileDialog('Images directory', isdir=True)
        ip = ImagesGPSParser(idir)
        PDFComposerScribus(ip.images, 3000)
except ImportError:
    if __name__ == '__main__':
        main()

    