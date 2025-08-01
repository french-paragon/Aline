/*This file is part of the project Aline

Copyright (C) 2022-2025 Paragon <french.paragon@gmail.com>

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

#include "model/editableitem.h"
#include "model/editableitemlist.h"

#include "model/editableitemmanager.h"

#include "utils/stringutils.h"

#include <QMetaObject>
#include <QEvent>

namespace Aline {

const std::array<int, 14> EditableItem::AllowedPropertiesTypeId = {QMetaType::Bool,
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
                                                          qMetaTypeId<QList<Aline::EditableItem*>>(),
                                                          qMetaTypeId<Aline::EditableItemList>()
														 };

const char* EditableItem::REF_PROP_NAME = "ref";
const char* EditableItem::TYPE_ID_NAME = "type_id";

const char* EditableItem::CHILDREN_PROP_NAME = "ref_childrens";

EditableItem::ManagedEditableItemReference& EditableItem::ManagedEditableItemReference::operator=(ManagedEditableItemReference const& other) {

	clearReference(); //clear previous refence

	if (_holder != nullptr) {
		_holder->_managedReferences.removeAll(this);
	}

	_holder = other._holder;
	_ref = other._ref;

	//no need to warn the new item the holder is now referring it, as the previous reference still exist!

	if (_holder != nullptr) {
		_holder->_managedReferences.push_back(this);
	}

	return *this;
}

bool EditableItem::ManagedEditableItemReference::setReference(QString url) {

	if (_holder == nullptr) {
		_ref = url;
		return true;
	}

	//manage the case of loading
	if (!_holder->hasBeenLoadedFromDisk()) {
		_ref = url;
		return true;
	}

	EditableItemManager* manager = _holder->getManager();

	if (manager == nullptr) {
		return false;
	}

	EditableItem* newTarget = manager->loadItemByUrl(url);

	if (newTarget == nullptr and !url.isEmpty()) {
		return false;
	}

	if (!_ref.isEmpty()) {
		if (manager != nullptr) {
			EditableItem* oldTarget = manager->loadItemByUrl(_ref);
			if (oldTarget != nullptr) {
				oldTarget->warnUnrefering(_holder->getFullRefUrl());
			}
		}
	}

	_ref.clear();
	if (newTarget != nullptr) {
		newTarget->warnRefering(_holder->getFullRefUrl());
	}
	_ref = url;
	return true;

}

void EditableItem::ManagedEditableItemReference::clearReference() {

	if (_holder == nullptr) {
		_ref.clear();
		return;
	}

	//manage the case of loading
	if (!_holder->hasBeenLoadedFromDisk()) {
		_ref.clear();
		return;
	}

	if (_holder != nullptr) {

		bool isSoleReference = true;

		for (ManagedEditableItemReference* ref : qAsConst(_holder->_managedReferences)) {
			if (ref == this) {
				continue;
			}

			if (ref->referedItem() == _ref) {
				isSoleReference = false;
				break;
			}
		}

		if (isSoleReference) {
			EditableItemManager* manager = _holder->getManager();

			if (manager != nullptr) {
				EditableItem* oldTarget = manager->loadItemByUrl(_ref);
				if (oldTarget != nullptr) {
					oldTarget->warnUnrefering(_holder->getFullRefUrl());
				}
			}
		}
	}

	_ref.clear();
}

QString EditableItem::simplifyRef(QString ref) {

	return StringUtils::simplifyRef(ref);

}

EditableItem::EditableItem(QString ref, Aline::EditableItemManager *parent) :
	QObject(parent),
	_ref(ref),
	_hasUnsavedChanged(false),
	_manager(parent),
	_parentItem(nullptr),
	_changeDetectionIsBlocked(false),
	_hasBeenLoadedFromDisk(true)
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
	_parentItem(parent),
	_changeDetectionIsBlocked(false),
	_hasBeenLoadedFromDisk(true)
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

	for (QString const& referentRef : qAsConst(_referentItems)) {
		EditableItem* referent = getManager()->loadItemByUrl(referentRef);

		QString oldUrl = getFullRefUrl();

		if (referent != nullptr) {
			referent->warnReferedRemoved(oldUrl);
			referent->updateDynamicReferences(oldUrl, "");
		}
	}
}

bool EditableItem::save() {

	if (_manager != nullptr) {
		bool status = _manager->saveItem(getRef());

		return status;
	}

	return false;
}

QString EditableItem::editAsTypeId() const {
	return getTypeId();
}

QString EditableItem::getRef() const
{
	return _ref;
}
QString EditableItem::getFullRefUrl() const {

	EditableItem* parent = getParentItem();

	if (parent == nullptr) {
		return getRef();
	}

	return parent->getFullRefUrl() + getManager()->refUrlSeparator() + getRef();

}

bool EditableItem::hasUnsavedChanges() const
{
	return _hasUnsavedChanged;
}

bool EditableItem::acceptChildrens() const {
	return false; //return false by default
}

void EditableItem::changeRef(QString const& newRef) {

	if (newRef != _ref) {
		QString oldRef = _ref;
		QString oldUrl = getFullRefUrl();
		_ref = newRef;
		QString newUrl = getFullRefUrl();

		for (QString const& referentRef : qAsConst(_referentItems)) {
			EditableItem* referent = getManager()->loadItemByUrl(referentRef);

			if (referent != nullptr) {
				referent->warnReferedRefChanges(oldUrl, newUrl);
				referent->updateDynamicReferences(oldUrl, newUrl);
			}
		}

		for (QString & referingRef : listRefering()) {
			EditableItem* refering = getManager()->loadItemByUrl(referingRef);

			if (refering != nullptr) {
				refering->warnReferentRefChanges(oldUrl, newUrl);
			}
		}

		QList<EditableItem*> childrens = getSubItems();

		for (EditableItem* & child : childrens) {
			child->treatParentRefChange(oldUrl, newUrl);
		}

		Q_EMIT refSwap(oldRef, _ref);
		Q_EMIT refChanged(_ref);
		newUnsavedChanges();
	}

}

void EditableItem::warnRefering(QString referentItemRef) {
	_referentItems.insert(referentItemRef);
	newUnsavedChanges();
}

void EditableItem::warnReferentRefChanges(QString referentItemOldRef, QString referentItemRef) {
	warnUnrefering(referentItemOldRef);
	warnRefering(referentItemRef);
}

void EditableItem::warnUnrefering(QString referentItemRef) {
	_referentItems.remove(referentItemRef);
	newUnsavedChanges();
}

QList<QString> EditableItem::listRefering() const {
	return QList<QString>();
}

void EditableItem::treatParentRefChange(QString oldParentUrl, QString newParentUrl) {

	QString oldUrl = oldParentUrl + getManager()->refUrlSeparator() + getRef();
	QString newUrl = newParentUrl + getManager()->refUrlSeparator() + getRef();

	for (QString const& referentRef : qAsConst(_referentItems)) {
		EditableItem* referent = getManager()->loadItemByUrl(referentRef);

		if (referent != nullptr) {
			referent->warnReferedRefChanges(oldUrl, newUrl);
		}
	}

	QList<EditableItem*> childrens = getSubItems();

	for (EditableItem* child : childrens) {
		child->treatParentRefChange(oldUrl, newUrl);
	}

}

void EditableItem::warnReferedRefChanges(QString oldRef, QString newRef) {
	Q_UNUSED(oldRef);
	Q_UNUSED(newRef);
	return;
}
void EditableItem::updateDynamicReferences(QString oldRef, QString newRef) {
	for (ManagedEditableItemReference* ref : qAsConst(_managedReferences)) {
		if (ref->referedItem() == oldRef) {
			ref->_ref = newRef;
		}
	}
}
void EditableItem::warnReferedRemoved(QString ref) {
	Q_UNUSED(ref);
	return;
}

void EditableItem::onVisibleStateChanged() {

	EditableItemManager* manager = getManager();

	if (manager != nullptr) {
		manager->refreshItemDataDisplay(getFullRefUrl()); //refresh displays
	}

	Q_EMIT visibleStateChanged(_ref);
}

void EditableItem::newUnsavedChanges() {

	if (_changeDetectionIsBlocked) {
		return;
	}

	if (autoSave()) {
		save();
		return;
	}

	bool previous = _hasUnsavedChanged;

	_hasUnsavedChanged = true;

	if (previous != _hasUnsavedChanged) {
		Q_EMIT unsavedStateChanged(_hasUnsavedChanged);
	}
}

void EditableItem::clearHasUnsavedChanges() {
	if (_hasUnsavedChanged) {

		QList<Aline::EditableItem*> items = getSubItems();

		for (EditableItem* sit : qAsConst(items)) {
			sit->clearHasUnsavedChanges();
		}

		_hasUnsavedChanged = false;
		Q_EMIT unsavedStateChanged(false);
	}
}

void EditableItem::onLoadingDone() {

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
	if (_manager == nullptr) {
		EditableItem* parent = getParentItem();

		if (parent != nullptr) {
			return parent->getManager();
		}
	}

	return _manager;
}

EditableItem *EditableItem::getTopLevelParentItem() const {

	EditableItem* parent = getParentItem();

	if (parent == nullptr) {
		return const_cast<EditableItem *>(this); //in case no parents are present, the top level item is the item
	}

	while (parent->getParentItem() != nullptr) {
		parent = parent->getParentItem();
	}

	return parent;
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

    item->changeRef(makeSubItemRefUniq(item->getRef()));

	_usedRef.insert(item->getRef());

	connect(item, &EditableItem::unsavedStateChanged, this, [this] (bool state) {
		if (state == true) newUnsavedChanges();
	});

	connect(item, &EditableItem::refSwap, this, [this] (QString oldRef, QString newRef) {
		_usedRef.remove(oldRef);
		_usedRef.insert(newRef);
	});

}

QList<Aline::EditableItem*> EditableItem::getSubItems() const {

	return QObject::findChildren<Aline::EditableItem*>(QString(), Qt::FindDirectChildrenOnly);

}

EditableItem* EditableItem::getSubItemByRef(QString const& ref) const {
	QList<Aline::EditableItem*> items = getSubItems();

	for (EditableItem* item : items) {
		if (item->getRef() == ref) {
			return item;
		}
	}

	return nullptr;
}

QStringList EditableItem::getSubItemsRefsList() const {
	QList<Aline::EditableItem*> items = getSubItems();

	QStringList lst;
	lst.reserve(items.size());

	for (EditableItem* item : items) {
		lst << item->getRef();
	}

	return lst;
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

bool EditableItem::hasBeenLoadedFromDisk() const
{
	return _hasBeenLoadedFromDisk;
}
void EditableItem::setupLoadingMode(bool loading) {
	_hasBeenLoadedFromDisk = !loading;
}

bool EditableItem::changeDetectionIsBlocked() const
{
	return _changeDetectionIsBlocked;
}

void EditableItem::blockChangeDetection(bool block_change_detection)
{
	_changeDetectionIsBlocked = block_change_detection;
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
			Q_EMIT labelRemoved(label);
		}

		for (QString const& label : labels) {
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
		Q_EMIT labelAdded(labelRef);
		return true;
	}
	return false;

}

bool EditableItem::removeLabel(QString const& labelRef) {

	if (hasLabel(labelRef)) {
		_labels.removeOne(labelRef);
		Q_EMIT labelRemoved(labelRef);
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
