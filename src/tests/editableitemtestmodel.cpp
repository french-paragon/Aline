#include "editableitemtestmodel.h"

#include <QtTest/QTest>
#include <QSignalSpy>

#include <QColor>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>

#include <QMetaObject>
#include <QMetaProperty>

#include "testeditableitemmanager.h"

namespace Aline {
namespace Tests {

void EditableItemTestModel::initTestCase() {

	_manager = new TestEditableItemManager;

}

void EditableItemTestModel::StaticPropertiesTypes() {

	recursivelyTestSubItemsStaticPropertiesTypes(_subject);

}

void EditableItemTestModel::StaticPropertiesChanges() {

	recursivelyTestSubItemsPropertiesChanges(_subject);

}

void EditableItemTestModel::cleanupTestCase() {

	if (_subject != nullptr) {
		delete _subject;
	}

	if (_manager != nullptr) {
		delete _manager;
	}

}

void EditableItemTestModel::recursivelyTestSubItemsStaticPropertiesTypes(EditableItem* subItem) {

	if (subItem == nullptr) {
		return;
	}

	const QMetaObject* mobj = subItem->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		if (!mobj->property(i).isStored(subItem)) {
			continue; //test only properties that need to be serialized
		}

		const char* prop = mobj->property(i).name();
		std::string str_msg = QString("Property named [%1] is stored but its type is not supported in Aline framework now !").toStdString();
		const char* msg = str_msg.c_str();

		int type_id = mobj->property(i).userType();

		QList<int> possibleTypesList(EditableItem::AllowedPropertiesTypeId.begin(), EditableItem::AllowedPropertiesTypeId.end());
		QVERIFY2(possibleTypesList.contains(type_id), msg);

		if (mobj->property(i).userType() == qMetaTypeId<Aline::EditableItem*>()) {

			QVariant val = subItem->property(prop);

			Aline::EditableItem* subItem = val.value<Aline::EditableItem*>();

			recursivelyTestSubItemsStaticPropertiesTypes(subItem);

			continue;

		} else if (mobj->property(i).userType() == qMetaTypeId<QList<Aline::EditableItem*>>()) {

			QVariant val = subItem->property(prop);

			QList<Aline::EditableItem*> list = val.value<QList<Aline::EditableItem*>>();

			for (Aline::EditableItem* subItem : list ) {
				recursivelyTestSubItemsStaticPropertiesTypes(subItem);
			}

			continue;

		}

	}

}

void EditableItemTestModel::recursivelyTestSubItemsPropertiesChanges(EditableItem* subItem) {

	if (subItem == nullptr) {
		return;
	}

	if (_subject->getHasUnsavedChanged()) {
		_subject->save();
	}

	QVERIFY2(!_subject->getHasUnsavedChanged(), "Initial saving of object failed !");

	const QMetaObject* mobj = subItem->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		if (!mobj->property(i).isStored(_subject)) {
			continue; //test only properties that need to be serialized
		}

		const char* prop = mobj->property(i).name();

		if (QString(prop) == EditableItem::REF_PROP_NAME) {
			continue; //don't alter the ref.
		}

		QVariant val = subItem->property(prop);

		if (mobj->property(i).userType() == qMetaTypeId<Aline::EditableItem*>()) {

			Aline::EditableItem* subItem = val.value<Aline::EditableItem*>();

			recursivelyTestSubItemsPropertiesChanges(subItem);

			continue;

		} else if (mobj->property(i).userType() == qMetaTypeId<QList<Aline::EditableItem*>>()) {

			QList<Aline::EditableItem*> list = val.value<QList<Aline::EditableItem*>>();

			for (Aline::EditableItem* subItem : list ) {
				recursivelyTestSubItemsPropertiesChanges(subItem);
			}

			continue;

		}

		QVariant nouveau = updateValue(val);

		if (!nouveau.isValid()) {
			continue;
		}

		QSignalSpy spy(_subject, &EditableItem::unsavedStateChanged);

		if (subItem->setProperty(prop, nouveau)) {

			QVERIFY2(_subject->getHasUnsavedChanged(), QString("Subject state should have changed when changing property named %1 !").arg(prop).toStdString().c_str());

			QCOMPARE(spy.count(), 1);
			QList<QVariant> arguments = spy.last();
			QVERIFY(arguments.at(0).toBool() == true);
		}

		_subject->save();

		QCOMPARE(spy.count(), 2);
		QList<QVariant> arguments = spy.last();
		QVERIFY(arguments.at(0).toBool() == false);

		QVERIFY2(!_subject->getHasUnsavedChanged(), "Intermediate saving of object failed !");

	}

}

QVariant EditableItemTestModel::updateValue(QVariant old) {

	switch (old.userType()) {
	case QMetaType::Bool :
		return !old.toBool();
	case QMetaType::Int :
		return (old.toInt() == 0) ? 1 : 0;
	case QMetaType::UInt :
		return (old.toUInt() == 0) ? (unsigned int) 1 : (unsigned int) 0;
	case QMetaType::QReal :
		return (old.toReal() <= 1) ? (qreal) 2 : (qreal) 0;
	case QMetaType::QString :
		return (old.toString().isEmpty()) ? QString("truc") : QString("");
	case QMetaType::QStringList :
		return old.toStringList() << QString("truc");
	case QMetaType::QPoint :
	{
		if (old.toPoint() == QPoint(0, 0)) {
			return QPoint(1, 1);
		} else {
			return QPoint(0,0);
		}
	}
	case QMetaType::QPointF :
	{
		if (old.toPointF().rx() <= 1) {
			return QPointF(2.0, 0);
		} else {
			return QPointF(0.0, 0);
		}
	}
	case QMetaType::QSize :
	{
		if (old.toSize() == QSize(0, 0)) {
			return QSize(1, 1);
		} else {
			return QSize(0,0);
		}
	}
	case QMetaType::QSizeF :
	{
		if (old.toSizeF().width() <= 1) {
			return QSizeF(2.0, 0);
		} else {
			return QSizeF(0.0, 0);
		}
	}
	case QMetaType::QColor :
	{
		QColor oldcolor = old.value<QColor>();
		QColor nouveau = old.value<QColor>();

		if (oldcolor.alpha() == 0) {
			nouveau.setAlpha(1);
		} else {
			nouveau.setAlpha(0);
		}

		return nouveau;
	}
	default:
		return QVariant();
	}

}


} // namespace Tests
} // namespace Aline
