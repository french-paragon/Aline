#ifndef ALINE_GLOBAL_H
#define ALINE_GLOBAL_H
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


#if defined(ALINE_LIBRARY)
#  define ALINE_EXPORT Q_DECL_EXPORT
#else
#  define ALINE_EXPORT Q_DECL_IMPORT
#endif

#endif // ALINE_GLOBAL_H
