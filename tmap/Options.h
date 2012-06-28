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

#ifndef TMAP_OPTIONS_H
#define TMAP_OPTIONS_H

#include <map>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

namespace tmap {
	
	class Options
	{
		public:
			static const std::string Transverse;
			static const std::string OutputFile;
			static const std::string ScaleDenominator;
			static const std::string MapnikStyle;
			static const std::string Longitude;
			static const std::string Latitude;
			static const std::string Projection;
			static const std::string RefLayer;
			static const std::string PartsCount;
			static const std::string Fonts;
			static const std::string MinLat;
			static const std::string MaxLat;
			static const std::string MinLon;
			static const std::string MaxLon;

		protected:
			std::map<std::string, std::string> pData;

		public:
			Options();
			void Add(const std::string& paramstring);
			void Add(const std::string &key, const std::string& val);
			bool Has(const std::string& key) const;
			bool Has(const std::vector<std::string>& keys) const;
			std::vector<std::string> Keys() const;

			std::string GetString(const std::string& key) const;

			template <class T>
			T Get(const std::string& key, const T& d) const
			{
				T ret;
				if(pData.find(key) == pData.end())
					return d;
				try
				{
					ret = boost::lexical_cast<T>(pData.find(key)->second);
				}
				catch(...)
				{
					ret = d;
				}
				return ret;
			}


			void Remove(const std::string& key);
			void Clear(){pData.clear();}
	};
	
} // namespace tmap

#endif // TMAP_OPTIONS_H
