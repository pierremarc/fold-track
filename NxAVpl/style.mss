@water:#7FA8D2;
@forest:#C1E6BD;
@swamp:#0CE84A;
@agri:#4BE80C;
@indus:#B4C9A9;
@residential:#F6F40E;
@building:#212121;
@motorway:#C1C1C1;
@road-in:#212121;
@road-out:#464646;
@red:#FF6161;
@terre:#F4F4F4;


@rot:270;

@font_crimson:"Crimson Text Roman","Serif";
@font_limousine:"Limousine Medium","Sans";
@font_limousine_bold:"Limousine Bold","Sans";

Map {
background-color:#ffffff;
}

#mer{
/* 	 polygon-fill:@red; */
	 polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/dot_blue.svg');
}

/*ajouter le fond terre pour supprimer le blanc quand c'est non assigné*/
#terre {
		polygon-fill:@terre;
}

#agricoles{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/dash_yellow.svg');
	/*line-width: 0.5;
	line-color: @red;*/
}

#foret{
/* 	polygon-fill:@forest; */
/* 	polygon-pattern-file:url('hatch00.png'); */
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_00.svg');
}

#marecages{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/dash_blue.svg');
}

#eau{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/dot_blue.svg');
}

#river{
	line-width: 0.4;
	line-color: @water;
	
	text-placement:line;
	text-min-path-length:500;
	text-min-distance:500;
	
	text-size:8;
	text-face-name:@font_crimson;
	text-name:"[name]";
	text-fill:@water;
}
#stream{
	line-width: 0.1;
	line-color: @water;
}


#indus{
	polygon-fill:@indus;
}

#residentielles{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_grey.svg');
}

/*#buildings{
	polygon-fill:@building;
}*/

#cimetieres{
	
/* 	polygon-fill:@building; */
	text-face-name:@font_limousine;
	text-size:8;
	text-name:"'+'";
	text-fill:#444;
	text-orientation:@rot;
}

#eglises{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_cult.svg');
	line-width: 0.1;
	line-color: @red;

}

#aeroports{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_plane.svg');
	line-width: 0.1;
	line-color: @red
}

#ancienbatiments{
	polygon-fill:#555555;
}


#badways{
	line-dasharray: 0.3, 0.6;
	line-width:0.3;
	line-color:@road-in;
}

#smallways{
	line-dasharray: 2, 1;
	line-width:0.3;
	line-color:@road-in;
}

#routesnormales{
/*	::outline {
	line-color: @road-out;
	line-width: 6;
	}*/
	line-color:@road-in;
	line-width:0.5;
}


#voiesrapides{
	line-color:@motorway;
	line-width:2;
}

.city-label{
	text-placement:point;
	text-face-name:@font_limousine;
	text-name:"[name]";
	text-fill:#AE5000;
}
/*#l_b{
	text-face-name:@font_limousine_bold;
	text-fill:#fff;
	text-size:18;
}*/
#cities_big{
	text-size:18;
}
#cities_medium{
	text-size:8;
}
#cities_small{
	text-face-name:@font_crimson;
	text-size:6;
}
#province{
	text-placement:interior;
	text-face-name:@font_limousine;
	text-size:26;
	text-name:"[name]";
	text-fill:#000;
}
