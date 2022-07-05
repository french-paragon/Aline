/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "datasourcebusyexception.h"

namespace Aline {

DataSourceBusyException::DataSourceBusyException(const QString &what) :
	_what(what.toStdString())
{

}
DataSourceBusyException::DataSourceBusyException(DataSourceBusyException const& other) :
	_what(other.what())
{

}

const char* DataSourceBusyException::what() const throw() {
	return _what.c_str();
}

void DataSourceBusyException::raise() const {
	throw *this;
}
DataSourceBusyException* DataSourceBusyException::clone() const {
	return new DataSourceBusyException(*this);
}

} // namespace Aline
