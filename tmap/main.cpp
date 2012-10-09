
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/image_util.hpp>
#include <string>
#include <iostream>
#include <map>
#include <exception>
#include <iomanip>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Utils.h"
#include "Options.h"
#include "Transform.h"

#include <mapnik/cairo_renderer.hpp>
#include <cairo-features.h>


namespace tmap{
	const std::string AllLayers = std::string("all");
	typedef std::pair<std::string, int> LayerRef_t;
}


int main(int ac, char ** av)
{
	tmap::Options params;
	for(int i(1); i < ac; ++i)
	{
		params.Add(std::string(av[i]));
	}

	double denom(params.Get(tmap::Options::ScaleDenominator, 100000.0));
	double parts(params.Get(tmap::Options::PartsCount, 1));

	std::string rLayerStr(tmap::AllLayers);
	if(params.Has(tmap::Options::RefLayer))
		rLayerStr = params.GetString(tmap::Options::RefLayer);

	if(params.Has(tmap::Options::Fonts))
	{
		std::string pFontStr(params.GetString(tmap::Options::Fonts));
		std::vector<std::string> fontVect;
		boost::algorithm::split(fontVect, pFontStr, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on );
		BOOST_FOREACH(std::string font, fontVect)
		{
			mapnik::freetype_engine::register_font(font);
		}
	}

	std::cerr<<"Fonts ======================="<<std::endl;
	const std::map<std::string,std::pair<int,std::string> >& font_map(mapnik::freetype_engine::get_mapping());
	for(std::map<std::string,std::pair<int,std::string> >::const_iterator fit(font_map.begin());
	    fit != font_map.end();
	    fit++)
	{
		std::cerr<<" "<<fit->first<< " "<< fit->second.first <<" "<< fit->second.second <<std::endl;
	}
//	std::cerr<<"=============================="<<std::endl;

	/// prepare the map
	mapnik::datasource_cache::instance()->register_datasources(tmap::mapnik_input_dir());
	mapnik::Map m;
	mapnik::load_map(m, params.GetString(tmap::Options::MapnikStyle));

	/// prep layers
	const int lc(m.layer_count());
	std::map<std::string, unsigned int> lnames;
	std::vector<std::string> pLayers;
	boost::algorithm::split(pLayers, rLayerStr, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on );
	if(pLayers.size() == 1 && pLayers.at(0) == tmap::AllLayers)
	{
		for(int i(0); i < lc; ++i)
		{
			mapnik::layer& l(m.layers().at(i));
			lnames[l.name()]=i;
		}
	}
	else
	{
		for(std::vector<std::string>::iterator plit(pLayers.begin()); plit != pLayers.end(); plit++)
		{
			std::string tl(*plit);
			if(tl[0] != '-')
			{
				for(int i(0); i < lc; ++i)
				{
					mapnik::layer& l(m.layers().at(i));
					if(tl == l.name())
						lnames[l.name()]=i;
				}
			}

		}
	}

	std::cerr<<"Layers ======================="<<std::endl;
	for(int i(0); i < lc; ++i)
	{
		mapnik::layer& l(m.layers().at(i));
		std::cerr<<" "<<l.name()<<std::endl;
	}
	std::cerr<<"=============================="<<std::endl;

//	/// get the extent
//	mapnik::box2d<double> bb;
//	bool firstLayer(true);
////	for(int i(0); i < lnames.size(); ++i)
//	BOOST_FOREACH(const tmap::LayerRef_t& kl, lnames)
//	{

//		mapnik::layer& l(m.layers().at(kl.second));
//		if(firstLayer)
//		{
//			firstLayer = false;
//			bb = l.envelope();
//		}
//		else
//		{
//			bb.expand_to_include(l.envelope());
//		}
//	}


//	double trans = params.Get(tmap::Options::Transverse, 1000.0);
//	if(params.Has(tmap::Options::Latitude)) // trans is height
//	{
//		// FIXME or FORGET IT

//		double lat(params.Get(tmap::Options::Latitude, bb.miny() + (bb.height() / 2.0)));
//		// we are looking for max and min y
//		double centerOnX(bb.minx() + (bb.width() / 2.0));

//		double maxy(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_NORTH, m.srs()).y);
//		double miny(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_SOUTH, m.srs()).y);
//		bb = mapnik::box2d<double>(bb.minx(),miny, bb.maxx(), maxy);

//	}
//	else if(params.Has(tmap::Options::Longitude))
//	{
//		double lon(params.Get(tmap::Options::Longitude, bb.minx() + (bb.width() / 2.0)));

//		double miny(params.Get(tmap::Options::MinLat, bb.miny()));
//		double maxy(params.Get(tmap::Options::MaxLat, bb.maxy()));

//		double maxx(std::max(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_WEST, m.srs()).x,
//				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_WEST, m.srs()).x));

//		double minx(std::min(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_EAST, m.srs()).x,
//				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_EAST, m.srs()).x));
//		bb = mapnik::box2d<double>(minx, miny,
//					   maxx, maxy);
//	}
//	else
//	{
//		throw std::string("No lat or lon to center the map on!");
//	}


	/// Now compute map size to fit this bounding box at required scale than resize map

	mapnik::box2d<double> bb(params.Get(tmap::Options::MinLon, -180.0f), params.Get(tmap::Options::MinLat, 180.0f),
				 params.Get(tmap::Options::MaxLon, -90.0f), params.Get(tmap::Options::MaxLat, 90.0f));
	double t0(tmap::bb_width(bb, 0, m.srs()));
	double t1(tmap::bb_width(bb, 2, m.srs()));
// 	double mapW(std::min(t0,t1) / denom);  // close to equator
// 	
	double mapW(bb.width() / denom);

	std::cerr<<"Covered Width: "<< std::min(t0,t1) <<" ("<< bb.width()<<")"<<std::endl;
//	double mapH(tmap::distance(mapnik::coord2d(bb.minx(), bb.miny()), mapnik::coord2d(bb.minx(), bb.maxy()), m.srs()) / denom);
	double mapH(bb.height() / denom);

	std::string out(params.GetString(tmap::Options::OutputFile));
	std::string type(*(mapnik::type_from_filename(out)));
	if(type != std::string("pdf") || parts == 1.0)
	{
		mapnik::Map targetMap(tmap::m2pt(mapW), tmap::m2pt(mapH));
		mapnik::load_map(targetMap, params.GetString(tmap::Options::MapnikStyle));

		for(unsigned int i(0); i < targetMap.layer_count(); i++)
		{
			targetMap.layers().at(i).set_active(false);
		}
		BOOST_FOREACH(const tmap::LayerRef_t& kl, lnames)
		{
			std::cerr<<"Activate layer for rendering: "<<kl.first<<std::endl;
			mapnik::layer& l(targetMap.layers().at(kl.second));
			l.set_active(true);
		}
		targetMap.zoom_to_box(bb);
		std::cerr<<"Map Width: "<< (double(targetMap.width()) * 0.03527)<< "cm" <<std::endl;


		if(type == std::string("png"))
			type = std::string("ARGB32");

		mapnik::save_to_cairo_file(targetMap, out, type);
	}
	else
	{

		double pt_map_width(tmap::m2pt(mapW));
		double pt_map_height(tmap::m2pt(mapH));
		double mstep(bb.height() / parts);
		double step(pt_map_height / parts);
		double rotate(params.Get(tmap::Options::Rotate, double(0.0)));
		double transx(params.Get(tmap::Options::TranslateX, double(0.0)));
		double padding(rotate * pt_map_width / 100.0f);
		double mpadding(rotate * bb.width() / 100.0f);

		pt_map_width = params.Get(tmap::Options::MapWidth, pt_map_width);

		std::cerr<<"pt_map_width\t "<<pt_map_width<<std::endl;
		std::cerr<<"pt_map_height\t "<<pt_map_height<<std::endl;
		std::cerr<<"mstep\t\t "<<mstep<<std::endl;
		std::cerr<<"step\t\t "<<step<<std::endl;
		std::cerr<<"rotate\t\t "<<rotate<<std::endl;

//		double offset(i * step);
//		std::cerr<<"\tOffset: "<<offset<<std::endl;
		ospi::Transform t;
		t.rotate(-rotate, ospi::Point(pt_map_width /double(2.0), pt_map_height /double(2.0)));
//		t.translate(transx - (pt_map_width / 2.0), -step/2.0);

//		Cairo::Matrix cm(Cairo::identity_matrix());
////		cm.translate(-pt_map_width /double(2.0), -pt_map_height /double(2.0));
//		cm.rotate(rotate * .0174532925199432958);
//		cm.translate(transx,0);

		for(double i(0); i < parts; i += 1.0)
		{
			std::cerr<<"Rendering part: "<<int(i)<<std::endl;
			std::cerr<<" Map interval: "<<(bb.maxy() - ((i+1) * mstep))<< " " << (bb.maxy() - (i * mstep)) <<std::endl;

//			mapnik::Map targetMap(pt_map_width * 2, step * 2);
			mapnik::Map targetMap(tmap::m2pt(mapW) , pt_map_height);
			mapnik::load_map(targetMap, params.GetString(tmap::Options::MapnikStyle));

			for(unsigned int j(0); j < targetMap.layer_count(); j++)
			{
				targetMap.layers().at(j).set_active(false);
			}
			BOOST_FOREACH(const tmap::LayerRef_t& kl, lnames)
			{
				mapnik::layer& l(targetMap.layers().at(kl.second));
				l.set_active(true);
			}
			mapnik::box2d<double> tbox(bb.minx() - mpadding, (bb.maxy() - ((i+1) * mstep)) - mpadding,
						   bb.maxx() + mpadding, (bb.maxy() - (i * mstep)) + mpadding);
			targetMap.zoom_to_box(bb);

			double offset(i * step);

			std::string prefix(boost::lexical_cast<std::string>(int(i)));
			prefix.append("_");
			prefix.append(out);

			Cairo::RefPtr<Cairo::Surface> surface;
			surface = Cairo::PdfSurface::create(prefix, pt_map_width, step);
			surface->set_device_offset(transx, -offset);

			Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
			mapnik::cairo_renderer<Cairo::Context> ren(targetMap, context);

//			context->rectangle(0,0,pt_map_width, step);
//			context->clip();

			const ospi::Transform::Matrix& mtx(t.getMatrix());
			Cairo::Matrix cm(mtx.m(1,1), mtx.m(1,2),
					 mtx.m(2,1), mtx.m(2,2),
					 mtx.m(3,1), mtx.m(3,3));
			context->set_matrix(cm);

			ren.apply();
			surface->finish();
		}



	}

	return 0;
}

