# split into parts


sdoc = params["INPUT"]
tdoc = params["OUTPUT"]
parts = int(params["PARTS"])
part = int(params["PART"])
sinfo = pdf_info.extract(sdoc)
bbox = sinfo["page_size"][0][1]
pcount = sinfo["page_count"]
swidth = bbox["width"]
sheight = bbox["height"]
theight = sheight / parts

print('Source Width: %s'%swidth)
print('Source Height: %s'%sheight)

print('Target Height: %s'%theight)

imposition_plan = []

#for i in range(0,parts):
offset = part * theight
print('Offset %s %s'%(part,offset))
imposition_plan.insert(0,{
	"target_document" : '%s_%s'%(part,tdoc),
	"target_page_width" : swidth,
	"target_page_height" : theight,
	"pages" : [
		{
		"source_document" : sdoc,
		"source_page" : 0,
		"crop_box" : {"left":0,"bottom":0,"width":swidth, "height":sheight},
		"translate" : [0,-offset],
		"rotate" : 0,
		"scale" : [1,1]
		}
		]
})
	
