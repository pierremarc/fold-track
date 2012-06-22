
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/image_util.hpp>
#include <string>
#include <iostream>
#include <map>
#include <exception>
#include <iomanip>

#include "Utils.h"
#include "Options.h"

#include <mapnik/cairo_renderer.hpp>
#include <cairo-features.h>


int main(int ac, char ** av)
{
	tmap::Options params;
	for(int i(1); i < ac; ++i)
	{
		params.Add(std::string(av[i]));
	}

	double denom(params.Get(tmap::Options::ScaleDenominator, 100000.0));
	double parts(params.Get(tmap::Options::PartsCount, 1));

	/// prepare the map
	mapnik::datasource_cache::instance()->register_datasources(tmap::mapnik_input_dir());
	mapnik::Map m;
	mapnik::load_map(m, params.GetString(tmap::Options::MapnikStyle));

	/// get the extent
	mapnik::box2d<double> bb;
	int lc(m.layer_count());
	std::map<std::string, unsigned int> lnames;
	for(int i(0); i < lc; ++i)
	{
		mapnik::layer& l(m.layers().at(i));
//		l.set_active(true);
//		bb.expand_to_include(l.envelope());
		lnames[l.name()]=i;
		std::cerr<<l.name()<< ": " <<l.envelope()<<std::endl;
	}

	if(params.Has(tmap::Options::RefLayer))
	{
		bb = m.layers().at(lnames[params.GetString(tmap::Options::RefLayer)]).envelope();
		std::cerr<< params.GetString(tmap::Options::RefLayer)<< " envelope => "<<bb<<std::endl;
	}

	double trans = params.Get(tmap::Options::Transverse, 1000.0);
	if(params.Has(tmap::Options::Latitude)) // trans is height
	{
		// FIXME

		double lat(params.Get(tmap::Options::Latitude, bb.miny() + (bb.height() / 2.0)));
		// we are looking for max and min y
		double centerOnX(bb.minx() + (bb.width() / 2.0));

		double maxy(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_NORTH, m.srs()).y);
		double miny(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_SOUTH, m.srs()).y);
		bb = mapnik::box2d<double>(bb.minx(),miny, bb.maxx(), maxy);

	}
	else if(params.Has(tmap::Options::Longitude))
	{
		double lon(params.Get(tmap::Options::Longitude, bb.minx() + (bb.width() / 2.0)));

		double maxx(std::max(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_WEST, m.srs()).x,
				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_WEST, m.srs()).x));

		double minx(std::min(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_EAST, m.srs()).x,
				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_EAST, m.srs()).x));
		bb = mapnik::box2d<double>(minx,bb.miny(),maxx,bb.maxy());
	}
	else
	{
		throw std::string("No lat or lon to center the map on!");
	}


	/// Now compute map size to fit this bounding box at required scale than resize map

	double t0(tmap::bb_width(bb, 0, m.srs()));
	double t1(tmap::bb_width(bb, 1, m.srs()));
	double mapW(std::min(t0,t1) / denom);

	double mapH(tmap::distance(mapnik::coord2d(bb.minx(), bb.miny()), mapnik::coord2d(bb.minx(), bb.maxy()), m.srs()) / denom);

	mapnik::Map targetMap(tmap::m2pt(mapW), tmap::m2pt(mapH));
	mapnik::load_map(targetMap, params.GetString(tmap::Options::MapnikStyle));
	for(int i(0); i < targetMap.layer_count(); ++i)
	{
		targetMap.layers().at(i).set_active(true);
	}
	targetMap.zoom_to_box(bb);
	std::cerr<<"Map Width: "<< targetMap.width() <<std::endl;

	std::string out(params.GetString(tmap::Options::OutputFile));
	std::string type(*(mapnik::type_from_filename(out)));
	if(type == std::string("png"))
		type = std::string("ARGB32");
	if(type != std::string("pdf") || parts == 1.0)
		mapnik::save_to_cairo_file(targetMap, out, type);
	else
	{
		std::ofstream file(out.c_str(), std::ios::out|std::ios::trunc|std::ios::binary);

		Cairo::RefPtr<Cairo::Surface> surface;
		Cairo::RefPtr<Cairo::Surface> rendersurface;
		unsigned width = targetMap.width();
		unsigned height = targetMap.height();
		unsigned pageheight = height / parts;
		surface = Cairo::PdfSurface::create(out,width,pageheight);
		rendersurface = Cairo::PdfSurface::create("/tmp/tmap_xxxx.pdf",width,height);

		Cairo::RefPtr<Cairo::Context> rendercontext = Cairo::Context::create(rendersurface);
		mapnik::cairo_renderer<Cairo::Context> ren(targetMap, rendercontext);
		std::cerr<<"Rendering "<<std::endl;
		ren.apply();

		std::cerr<<"Splitting "<<std::endl;
		Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
		for(int i(0); i<parts; i++)
		{
			std::cerr<<"Page "<<(i+1)<<std::endl;
			context->set_source(rendersurface, 0, i * pageheight);
			context->paint();
			context->show_page();
		}


		rendersurface->finish();
		surface->finish();
	}

	return 0;
}

