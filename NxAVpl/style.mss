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



@font_reg:"Crimson Text Roman","Ubuntu Regular","Arial Regular","DejaVu Sans Book";

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
/*	::outline {
	line-color: @road-out;
	line-width: 6;
	}*/
	line-color:@road-in;
	line-width:1.3;
}


#routesnormales{
  text-placement:line;
  text-face-name:@font_reg;
    text-name:"[NAME]";
    text-fill:#444;
  }
}