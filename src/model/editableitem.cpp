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
	_manager(parent),
	_parentItem(nullptr)
{

	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::newUnsavedChanges);
	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::onVisibleStateChanged);
	connect(this, &EditableItem::unsavedStateChanged, this, &EditableItem::onVisibleStateChanged);

}

EditableItem::EditableItem(QString ref, Aline::EditableItem *parent) :
	QObject(parent),
	_ref(ref),
	_manager(nullptr),
	_parentItem(parent)
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
		QString oldRef = _ref;
		_ref = newRef;
		emit refSwap(oldRef, _ref);
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

QString EditableItem::makeSubItemRefUniq(QString const& subItemRef) const {

	if (!_usedRef.contains(subItemRef)) {
		return subItemRef;
	}

	QString model("%1%2");
	model = model.arg(subItemRef);
	QString proposed_ref;

	int i = 1;

	do {
		proposed_ref = model.arg(i);
	} while (_usedRef.contains(proposed_ref));

	return proposed_ref;

}

void EditableItem::removeSubItemRef(EditableItem* item) {
	_usedRef.remove(item->getRef());
}

EditableItemManager *EditableItem::getManager() const
{
	return _manager;
}

EditableItem *EditableItem::getParentItem() const
{
	return _parentItem;
}

void EditableItem::insertSubItem(EditableItem* item) {

	if (item == nullptr) {
		return;
	}

	if (item->getParentItem() != nullptr) {
		if (item->getParentItem() != this) {

			item->setParentItem(this);

		}
	}

	_usedRef.insert(item->getRef());

}

QList<Aline::EditableItem*> EditableItem::getSubItems() const {

	return QObject::findChildren<Aline::EditableItem*>(QString(), Qt::FindDirectChildrenOnly);

}

void EditableItem::setParentItem(EditableItem* parent) {

	if (_parentItem != nullptr) {
		if (_parentItem != parent) {
			_parentItem = parent;
			setParent(parent);
		}
	}

}

} // namespace Aline
