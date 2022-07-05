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

#include "editableitemmanagerfactory.h"

#include <QMap>

namespace Aline {

EditableItemManagerFactory::EditableItemManagerFactory(QObject *parent) : QObject(parent)
{
	_noFunctionDescr._projectDescr = tr("Not Found");
	_noFunctionDescr._longDescr = tr("Not Found");
}

EditableItemManager* EditableItemManagerFactory::createManagerWithRegistredFunction(QString const& fName, QObject* parent) const {

	if (!_simpleFuncs.contains(fName)) {
		return nullptr;
	}

	return _simpleFuncs[fName](parent);

}
EditableItemManager* EditableItemManagerFactory::createManagerWithRegistredFunctionAndGuiContext(QString const& fName, MainWindow* win, QObject* parent, bool fallBackToUnaware) const {

	if (!_guiAwareFuncs.contains(fName)) {

		if (fallBackToUnaware) {

			if (_simpleFuncs.contains(fName)) {
				return _simpleFuncs[fName](parent);
			}

		}

		return nullptr;

	}

	return _guiAwareFuncs[fName](parent, win);

}

EditableItemManager* EditableItemManagerFactory::createManagerWithRegistredFunctionAndStringSource(QString const& fName, QString source, QObject* parent) const {

	if (!_stringSourceFuncs.contains(fName)) {
		return nullptr;
	}

	return _stringSourceFuncs[fName](parent, source);

}

EditableItemManagerFactory::funcDescr EditableItemManagerFactory::getSimpleFuncDescr(QString const& fName) const {

	if (_simpleFuncsDescriptions.contains(fName)) {
		return _simpleFuncsDescriptions[fName];
	}

	return _noFunctionDescr;

}

EditableItemManagerFactory::funcDescr EditableItemManagerFactory::getGuiAwareFuncDescr(QString const& fName) const {

	if (_guiAwareFuncsDescriptions.contains(fName)) {
		return _guiAwareFuncsDescriptions[fName];
	}

	return _noFunctionDescr;
}

EditableItemManagerFactory::funcDescr EditableItemManagerFactory::getStringSourceFuncDescr(QString const& fName) const {

	if (_stringSourceFuncsDescriptions.contains(fName)) {
		return _stringSourceFuncsDescriptions[fName];
	}

	return _noFunctionDescr;
}

void EditableItemManagerFactory::registerSimpleFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*)> const& func) {

	if (_simpleFuncs.contains(fName)) {
		return;
	}

	_simpleFuncs.insert(fName, func);
	_simpleFuncsDescriptions.insert(fName, descr);

}

void EditableItemManagerFactory::registerGuiAwareFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*, MainWindow*)> const& func) {

	if (_guiAwareFuncs.contains(fName)) {
		return;
	}

	_guiAwareFuncs.insert(fName, func);
	_guiAwareFuncsDescriptions.insert(fName, descr);

}

void EditableItemManagerFactory::registerStringSourceFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*, QString)> const& func) {

	if (_stringSourceFuncs.contains(fName)) {
		return;
	}

	_stringSourceFuncs.insert(fName, func);
	_stringSourceFuncsDescriptions.insert(fName, descr);

}

bool EditableItemManagerFactory::containFunc(QString const& fname, int type) const{

	switch (type) {
	case SIMPLE_FUNC:
		return _simpleFuncs.contains(fname);
	case GUI_FUNC:
		return _guiAwareFuncs.contains(fname);
	case STRING_FUNC:
		return _stringSourceFuncs.contains(fname);
	default:
		break;
	}

	return false;

}

} // namespace Aline
