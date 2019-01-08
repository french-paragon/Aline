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
