#include "model/editableitem.h"

#include "model/editableitemmanager.h"

#include "utils/stringutils.h"

#include <QMetaObject>
#include <QEvent>

namespace Aline {

const QList<int> EditableItem::AllowedPropertiesTypeId = {QMetaType::Bool,
														  QMetaType::Int,
														  QMetaType::UInt,
														  QMetaType::QReal,
														  QMetaType::QString,
														  QMetaType::QStringList,
														  QMetaType::QPoint,
														  QMetaType::QPointF,
														  QMetaType::QSize,
														  QMetaType::QSizeF,
														  QMetaType::QColor,
														  qMetaTypeId<Aline::EditableItem*>(),
														  qMetaTypeId<QList<Aline::EditableItem*>>()
														 };

const QString EditableItem::REF_PROP_NAME = "ref";
const QString EditableItem::TYPE_ID_NAME = "type_id";

const QString EditableItem::CHILDREN_PROP_NAME = "ref_childrens";


QString EditableItem::simplifyRef(QString ref) {

	return StringUtils::simplifyRef(ref);

}

EditableItem::EditableItem(QString ref, Aline::EditableItemManager *parent) :
	QObject(parent),
	_ref(ref),
	_hasUnsavedChanged(false),
	_manager(parent),
	_parentItem(nullptr),
	_block_change_detection(false)
{
	qRegisterMetaType<Aline::EditableItem*>();

	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::newUnsavedChanges);
	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::onVisibleStateChanged);
	connect(this, &EditableItem::unsavedStateChanged, this, &EditableItem::onVisibleStateChanged);

}

EditableItem::EditableItem(QString ref, Aline::EditableItem *parent) :
	QObject(parent),
	_ref(ref),
	_hasUnsavedChanged(false),
	_manager(nullptr),
	_parentItem(parent)
{
	qRegisterMetaType<Aline::EditableItem*>();

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

		return status;
	}

	return false;
}

QString EditableItem::getRef() const
{
	return _ref;
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

	if (_block_change_detection) {
		return;
	}

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

		for (EditableItem* sit : getSubItems()) {
			sit->clearHasUnsavedChanges();
		}

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
		i++;
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

	if (item->getParentItem() != this) {

		item->setParentItem(this);

	}

	_usedRef.insert(item->getRef());

	connect(item, &EditableItem::unsavedStateChanged, [this] (bool state) {
		if (state == true) newUnsavedChanges();
	});

}

QList<Aline::EditableItem*> EditableItem::getSubItems() const {

	return QObject::findChildren<Aline::EditableItem*>(QString(), Qt::FindDirectChildrenOnly);

}

QStringList EditableItem::getFileReferencePropertiesName() const {
	return {};
}

void EditableItem::setParentItem(EditableItem* parent) {

	if (_parentItem != nullptr) {
		if (_parentItem != parent) {
			_parentItem = parent;
			setParent(parent);
		}
	}

}

void EditableItem::blockChangeDetection(bool block_change_detection)
{
	_block_change_detection = block_change_detection;
}

QStringList EditableItem::getLabels() const
{
	return _labels;
}

void EditableItem::setLabels(const QStringList &labels)
{
	if (signalsBlocked()) {
		_labels = labels;
	} else {

		while (!_labels.empty()) {
			QString label = _labels.back();
			_labels.pop_back();
			emit labelRemoved(label);
		}

		for (QString label : labels) {
			addLabel(label);
		}
	}
}

bool EditableItem::hasLabel(QString const& labelRef) const {
	return _labels.contains(labelRef);
}

bool EditableItem::addLabel(QString const& labelRef) {

	if (!hasLabel(labelRef)) {
		_labels.push_back(labelRef);
		emit labelAdded(labelRef);
		return true;
	}
	return false;

}

bool EditableItem::removeLabel(QString const& labelRef) {

	if (hasLabel(labelRef)) {
		_labels.removeOne(labelRef);
		emit labelRemoved(labelRef);
		return true;
	}
	return false;

}

bool EditableItem::event(QEvent *e) {

	if (e->type() == QEvent::DynamicPropertyChange) {
		newUnsavedChanges();
		return true;
	}

	return QObject::event(e);
}

} // namespace Aline
