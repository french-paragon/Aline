#include "editorfactory.h"

#include "editor.h"

namespace Aline {

EditorFactory::EditorFactory(QObject *parent) :
	QObject(parent),
	_typeId(""),
	_typeName(""),
	_hasEditableTypesCached(false)
{

}

QString EditorFactory::getItemTypeId() const {

	if (_typeId == "") {

		//get the type name.
		Editor* temp = createItem(nullptr);
		_typeId = temp->getTypeId();

		delete temp;

	}

	return _typeId;

}

QString EditorFactory::getItemTypeName() const {

	if (_typeName == "") {

		//get the type name.
		Editor* temp = createItem(nullptr);
		_typeName = temp->getTypeName();

		delete temp;

	}

	return _typeName;

}

QStringList EditorFactory::editableTypes() const {

	if (!_hasEditableTypesCached) {

		Editor* temp = createItem(nullptr);
		_editableTypes = temp->editableTypes();

		delete temp;
	}

	return _editableTypes;

}

} // namespace Aline
