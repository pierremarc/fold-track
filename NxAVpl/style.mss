@water:#91C3CB;
@forest:#C1E6BD;
@swamp:#0CE84A;
@agri:#4BE80C;
@indus:#AFB9AB;
@residential:#F6F40E;
@building:#000000;
@motorway:#C1C1C1;
@road-in:#000000;
@road-out:#464646;

@rot:270;

@font_crimson:"Crimson Text Roman","Serif";
@font_limousine:"Limousine Medium","Sans";

Map {
  background-color:white;
}


#agricoles{
 	polygon-fill:@agri;
}

#foret{
/* 	polygon-fill:@forest; */
/* 	polygon-pattern-file:url('hatch00.png'); */
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_00.svg');
}

#marecages{
	polygon-fill:@swamp;
}

#eau{
	polygon-fill:@water;
}

#indus{
	polygon-fill:@indus;
}

#residentielles{
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_grey.svg');
}

#buildings{
	polygon-fill:@building;
}

#cimetieres{
	
/* 	polygon-fill:@building; */
	text-face-name:@font_limousine;
	text-size:13;
	text-name:"'+'";
	text-fill:#000;
	text-orientation:@rot;
}

#eglises{
	polygon-fill:red;
}
#ancienbatiments{
	polygon-fill:#ff00aa;
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

#cities_big{
	text-placement:point;
	text-face-name:@font_limousine;
	text-size:18;
	text-name:"[name]";
	text-fill:#000;
}
#cities_medium{
	text-placement:point;
	text-face-name:@font_limousine;
	text-size:8;
	text-name:"[name]";
	text-fill:#000;
}
#cities_small{
	text-placement:point;
	text-face-name:@font_limousine;
	text-size:6;
	text-name:"[name]";
	text-fill:#000;
}
#province{
	text-placement:interior;
	text-face-name:@font_limousine;
	text-size:26;
	text-name:"[name]";
	text-fill:#000;
}
