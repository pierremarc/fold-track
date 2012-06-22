@water:#15A0E7;
@forest:#0A7108;
@swamp:#0CE84A;
@agri:#4BE80C;
@indus:#AFB9AB;
@residential:#F6F40E;
@building:#000000;
@motorway:#FF5541;
@road-in:#C2C2C2;
@road-out:#464646;

Map {
  background-color:white;
}


#agricoles{
	polygon-fill:@agri;
}

#foret{
	polygon-fill:@forest;
/* 	polygon-pattern-file:url('hatch00.png'); */
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


#voiesrapides{
	line-color:@motorway;
	line-width:2;
}

#routesnormales{
	::outline {
	line-color: @road-out;
	line-width: 6;
	}
	line-color:@road-in;
	line-width:3;
}