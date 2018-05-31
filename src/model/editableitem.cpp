#include "model/editableitem.h"

#include "model/editableitemmanager.h"

#include "utils/stringutils.h"

#include <QMetaObject>

namespace Aline {

const QString EditableItem::REF_PROP_NAME = "ref";
const QString EditableItem::TYPE_ID_NAME = "type_id";

const QString EditableItem::CHILDREN_PROP_NAME = "ref_childrens";


QString EditableItem::simplifyRef(QString ref) {

	return StringUtils::simplifyRef(ref);

}

EditableItem::EditableItem(QString ref, Aline::EditableItemManager *parent) :
	QObject(parent),
	_ref(ref),
	_manager(parent)
{

	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::newUnsavedChanges);
	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::onVisibleStateChanged);
	connect(this, &EditableItem::unsavedStateChanged, this, &EditableItem::onVisibleStateChanged);

}

bool EditableItem::autoSave() const {
	return false;
}

void EditableItem::suppress() {
	//default implementation do noting.
}

bool EditableItem::save() {

	if (_manager != nullptr) {
		bool status = _manager->saveItem(getRef());

		if (status && _hasUnsavedChanged) {
			_hasUnsavedChanged = false;
			emit unsavedStateChanged(false);
		}
	}

	return false;
}

QString EditableItem::getRef() const
{
	return _ref;
}

QVector<QString> EditableItem::getChildrenItemsRefs() const {
	if (_manager != nullptr) {
		return _manager->listChildren(getRef());
	}
	return QVector<QString>();
}

bool EditableItem::getHasUnsavedChanged() const
{
	return _hasUnsavedChanged;
}

bool EditableItem::acceptChildrens() const {
	return false; //return false by default
}

void EditableItem::changeRef(QString const& newRef) {

	if (newRef != _ref) {
		_ref = newRef;
		emit refChanged(_ref);
	}

}

void EditableItem::onVisibleStateChanged() {
	emit visibleStateChanged(_ref);
}

void EditableItem::newUnsavedChanges() {

	if (autoSave()) {
		save();
		return;
	}

	bool previous = _hasUnsavedChanged;

	_hasUnsavedChanged = true;

	if (previous != _hasUnsavedChanged) {
		emit unsavedStateChanged(_hasUnsavedChanged);
	}
}

void EditableItem::clearHasUnsavedChanges() {
	if (_hasUnsavedChanged) {
		_hasUnsavedChanged = false;
		emit unsavedStateChanged(false);
	}
}

} // namespace Aline
