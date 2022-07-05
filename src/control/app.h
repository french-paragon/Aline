#ifndef ALINE_APP_H
#define ALINE_APP_H

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

#include "../aline_global.h"

#include <QApplication>

namespace Aline {

class ALINE_EXPORT App : public QApplication
{
public:
	App(int &argc, char** argv);

	virtual bool start(QString appCode = "default");

protected:

	QString _appCode;
};

} // namespace Aline

#endif // APP_H
