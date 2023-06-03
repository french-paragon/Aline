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

class EditableItemActionsManager;

class ALINE_EXPORT App : public QApplication
{
	Q_OBJECT
public:

	static App* getAppInstance();

	App(int &argc, char** argv);

	virtual bool start(QString appCode = "default");

	/*!
	 * \brief hasActionManager indicate if a dedicated action manager is installed for a type
	 * \param typeRef the type ref
	 * \return true if a manager is installed, false otherwise.
	 */
	bool hasActionManager(QString const& typeRef) const;

	/*!
	 * \brief getManagerForType return the action manager for a given type
	 * \param typeRef the type ref
	 * \return the installed action manager for the type, or the default manager if none is available.
	 */
	EditableItemActionsManager* getManagerForType(QString const& typeRef) const;

	/*!
	 * \brief installManagerForType install a manager for a given type, in no manager has been installed yet.
	 * \param typeRef the type to install the manager for
	 * \param manager the manager to install
	 * \return true if th manager was installed, false otherwise
	 *
	 * The app takes ownership of the manager
	 */
	bool installManagerForType(QString const& typeRef, EditableItemActionsManager* manager);

protected:

	QString _appCode;

	EditableItemActionsManager* _defaultActionManager;
	QMap<QString, EditableItemActionsManager*> _installedActionManager;
};

} // namespace Aline

#endif // APP_H
