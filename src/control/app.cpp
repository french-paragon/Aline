/*This file is part of the project Aline

Copyright (C) 2022-2023 Paragon <french.paragon@gmail.com>

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
#include "model/editableitemfactory.h"

#include <QTemporaryDir>

namespace Aline {

App* App::getAppInstance() {
	return qobject_cast<App*>(QApplication::instance());
}

EditableItemFactoryManager* App::getAppEditableItemFactoryManager() {

	static EditableItemFactoryManager* manager = nullptr;

	if (manager == nullptr and getAppInstance() != nullptr) {
		manager = new EditableItemFactoryManager(getAppInstance()); //build a global EditableItemFactoryManager which will be deleted when the app instance is deleted
	}

	return manager;

}

App::App(int & argc, char **argv) :
	QApplication(argc, argv),
	_tmp_dir(nullptr)
{
	_defaultActionManager = new EditableItemActionsManager(this); //default action manager
}

App::~App() {

	if (_tmp_dir != nullptr) {
		delete _tmp_dir;
	}
}

bool App::start(QString appCode) {
	_appCode = appCode;
	Q_EMIT startupStarted();
	bool code = start_actions();
	if (code) {
		Q_EMIT startupFinished();
	}
	return code;
}

bool App::start_actions() {
	return true;
}

bool App::hasActionManager(QString const& typeRef) const {
	return _installedActionManager.contains(typeRef);
}
EditableItemActionsManager* App::getActionsManagerForType(QString const& typeRef) const {
	return _installedActionManager.value(typeRef, _defaultActionManager);
}
bool App::installActionsManagerForType(QString const& typeRef, EditableItemActionsManager* manager) {
	if (hasActionManager(typeRef)) {
		return false;
	}

	manager->setParent(this);
	_installedActionManager.insert(typeRef, manager);

	return true;
}

QString App::getAppTempDirectory() const {

	if (_tmp_dir == nullptr) {
		_tmp_dir = new QTemporaryDir();
	}

	return _tmp_dir->path();

}

} // namespace Aline
