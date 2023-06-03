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

#include "app.h"

#include "editableitemactionsmanager.h"

namespace Aline {

App* App::getAppInstance() {
	return qobject_cast<App*>(QApplication::instance());
}

App::App(int & argc, char **argv) :
	QApplication(argc, argv)
{
	_defaultActionManager = new EditableItemActionsManager(this); //default action manager
}

bool App::start(QString appCode) {
	_appCode = appCode;
	return true;
}

bool App::hasActionManager(QString const& typeRef) const {
	return _installedActionManager.contains(typeRef);
}
EditableItemActionsManager* App::getManagerForType(QString const& typeRef) const {
	return _installedActionManager.value(typeRef, _defaultActionManager);
}
bool App::installManagerForType(QString const& typeRef, EditableItemActionsManager* manager) {
	if (hasActionManager(typeRef)) {
		return false;
	}

	manager->setParent(this);
	_installedActionManager.insert(typeRef, manager);

	return true;
}

} // namespace Aline
