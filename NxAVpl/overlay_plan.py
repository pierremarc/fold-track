# split into parts


sdocs = params["INPUT"].split(',')

tdoc = params["OUTPUT"]
sinfo = pdf_info.extract('%s.pdf'%(sdocs[0],))
bbox = sinfo["page_size"][0][1]
pcount = sinfo["page_count"]
swidth = bbox["width"]
sheight = bbox["height"]

print('Source Width: %s'%swidth)
print('Source Height: %s'%sheight)

imposition_plan = []
pages = []
for sdoc in sdocs:
	pages.append({
			"source_document" : '%s.pdf'%sdoc,
			"source_page" : 0,
			"crop_box" : {"left":0,"bottom":0,"width":swidth, "height":sheight},
			"translate" : [0,0],
			"rotate" : 0,
			"scale" : [1,1]
			})
	
imposition_plan.insert(0,{
		"target_document" : tdoc,
		"target_page_width" : swidth,
		"target_page_height" : sheight,
		"pages" : pages
	})