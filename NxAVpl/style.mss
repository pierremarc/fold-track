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
/* 	polygon-fill:@agri; */
	/*line-color:@motorway;
	line-width:1;*/
	polygon-pattern-file:url('/home/pierre/System/src/fold-track/NxAVpl/hatch_00.svg');
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
	polygon-fill:@residential;
}

#buildings{
	polygon-fill:@building;
}

#cimetieres{
	
/* 	polygon-fill:@building; */
	text-face-name:@font_limousine;
	text-size:13;
	text-name:"'Cim.'";
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
	line-dasharray: 0.3 0.6;
	line-width:0.3;
	line-color:@road-in;
}

#smallways{
	line-dasharray: 2 1;
	line-width:0.3;
	line-color:@road-in;
}

#routesnormales{
/*	::outline {
	line-color: @road-out;
	line-width: 6;
	}*/
	line-color:@road-in;
	line-width:1.3;
}

#roadname{
	text-placement:line;
	text-face-name:@font_crimson;
	text-size:9;
	text-name:"[name]";
	text-fill:#444;
	text-dy:4;
/* 	text-orientation:@rot; */
}

#voiesrapides{
	line-color:@motorway;
	line-width:2;
}

#municipality{
	text-placement:interior;
	text-face-name:@font_limousine;
	text-size:18;
	text-name:"[name]";
	text-fill:#000;
	text-orientation:@rot;
}
#province{
	text-placement:interior;
	text-face-name:@font_limousine;
	text-size:26;
	text-name:"[name]";
	text-fill:#000;
	text-orientation:@rot;
}
