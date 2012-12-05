/***************************************************************************
 *   Copyright (C) 2012 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "Transform.h"

#include <iostream>
#include <cmath>

namespace ospi {


	Rectangle::Rectangle()
	{
		resize(4);
	}

	Rectangle::Rectangle(const Point& c1, const Point& c2)
	{
		resize(4);
		at(BL) = c1;
		at(TR) = c2;
		normalize();
	}

	Rectangle Rectangle::unite(const Rectangle &o)
	{
		Rectangle r(*this);
		r.at(BL) = Point(std::min(at(BL).x, o.at(BL).x), std::min(at(BL).y, o.at(BL).y));
		r.at(TR) = Point(std::max(at(TR).x, o.at(TR).x), std::max(at(TR).y, o.at(TR).y));
		return Rectangle(r.normalize());
	}

	Rectangle& Rectangle::united(const Rectangle &o)
	{
		this->operator =(unite(o));
		return (*this);
	}

	Rectangle& Rectangle::normalize()
	{
		Point c1(at(BL));
		Point c2(at(TR));
		if(c1.x <= c2.x)
		{
			if(c1.y <= c2.y)
			{
				at(BL) = c1;
				at(TR) = c2;
			}
			else
			{
				at(BL) = Point(c1.x, c2.y);
				at(TR) = Point(c2.x, c1.y);
			}
		}
		else
		{
			if(c1.y <= c2.y)
			{
				at(BL) = Point(c2.x, c1.y);
				at(TR) = Point(c1.x, c2.y);
			}
			else
			{
				at(BL) = c2;
				at(TR) = c1;
			}
		}

		at(TL) = Point(at(BL).x, at(TR).y);
		at(BR) = Point(at(TR).x, at(BL).y);

		return (*this);
	}

	const Rectangle& Rectangle::operator =(const Rectangle& o)
	{
		resize(4);
		for(unsigned int i(0); i < 4; i++)
		{
			at(i) = o.at(i);
		}
	}

	Transform Transform::fromString(const std::string &tm)
	{
		// TODO - if found useful
		return Transform();
	}

	std::string Transform::toCMString() const
	{
		return m.toString().append(" cm");
	}

	Transform& Transform::translate(trx_double_t dx, trx_double_t dy)
	{
		Matrix transMat;
		transMat.m(3,1) = dx;
		transMat.m(3,2) = dy;
		m *= transMat;
		return (*this);
	}

	Transform& Transform::rotate(trx_double_t r, const Point &origin)
	{
		trx_double_t rGrad(r * trx_double_t(3.14159) / trx_double_t(180.0));
#ifndef WITH_GMP
                trx_double_t cosR(cos(rGrad));
                trx_double_t sinR(sin(rGrad));
#else
		trx_double_t cosR(cos(rGrad.get_d()));
		trx_double_t sinR(sin(rGrad.get_d()));
#endif
		Matrix rotMat;
		if(!origin.IsOrigin())
		{
			Matrix tr1;
			tr1.m(3,1) = -origin.x;
			tr1.m(3,2) = -origin.y;
			rotMat *= tr1;
//			std::cerr<<"\tTR1 "<< rotMat.toString()<<std::endl;

			Matrix tr2;
			tr2.m(1,1) = cosR;
			tr2.m(1,2) = sinR;
			tr2.m(2,1) = -sinR;
			tr2.m(2,2) = cosR;
			rotMat *= tr2;
//			std::cerr<<"\tTR2 "<< rotMat.toString()<<std::endl;

			Matrix tr3;
			tr3.m(3,1) = origin.x;
			tr3.m(3,2) = origin.y;
			rotMat *= tr3;
//			std::cerr<<"\tTR3 "<< rotMat.toString()<<std::endl;
		}
		else
		{
			rotMat.m(1,1) = cosR;
			rotMat.m(1,2) = sinR;
			rotMat.m(2,1) = -sinR;
			rotMat.m(2,2) = cosR;
		}
		m *= rotMat;

		return (*this);

	}

	Transform& Transform::scale(trx_double_t sx, trx_double_t sy, const Point &origin)
	{
		Matrix scaleMat;
		if(!origin.IsOrigin())
		{
			Matrix tr1;
			tr1.m(3,1) = -origin.x;
			tr1.m(3,2) = -origin.y;
			scaleMat *= tr1;

			Matrix tr2;
			tr2.m(1,1) = sx;
			tr2.m(2,2) = sy;
			scaleMat *= tr2;

			Matrix tr3;
			tr3.m(3,1) = origin.x;
			tr3.m(3,2) = origin.y;
			scaleMat *= tr3;
		}
		else
		{
			scaleMat.m(1,1) = sx;
			scaleMat.m(2,2) = sy;
		}
		m *= scaleMat;
		return (*this);
	}

	void Transform::map(Point &p) const
	{
		trx_double_t rx(p.x * m.m(1,1) + p.y * m.m(2,1) + m.m(3,1));
		trx_double_t ry(p.x * m.m(1,2) + p.y * m.m(2,2) + m.m(3,2));

		p.x = rx;
		p.y = ry;
	}

	void Transform::map(Rectangle &r) const
	{
		for(unsigned int i(0); i < 4; ++i)
		{
			map(r[i]);
		}
		r.normalize();
	}

	
} // namespace ospi
