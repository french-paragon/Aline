#include <QTest>

#include <QObject>

#include "../../src/model/editableitemfactory.h"

#include "../../src/tests/testeditableitemmanager.h"
#include "../../src/tests/dummyeditableitem.h"
#include "../../src/tests/referingeditableitem.h"

#include "../../src/utils/jsonutils.h"

class TestEditableItemLifeCycle : public QObject
{
	Q_OBJECT
public :
private Q_SLOTS:
	void initTestCase();

	void refSet();

	void refChange();
	void referingChange();
	void referedDeleted();

	void subItemRefered();

	void editableItemJsonEncoding();

	void cleanupTestCase();

private:

	Aline::Tests::TestEditableItemManager* itemManager;

	Aline::EditableItem* dummy1;
	Aline::EditableItem* dummy2;

	Aline::Tests::ReferingEditableItem* refering;

	Aline::Tests::DummyEnrichedEditableItem* dummyRich1;
	Aline::Tests::DummyEnrichedEditableItem* dummyRich2;

};

void TestEditableItemLifeCycle::initTestCase() {
	itemManager = new Aline::Tests::TestEditableItemManager(this);

	Aline::EditableItemFactoryManager* factoryManager = itemManager->factoryManager();
	factoryManager->installFactory(new Aline::Tests::DummyEditableItemFactory());
	factoryManager->installFactory(new Aline::Tests::DummyEnrichedEditableItemFactory());
	factoryManager->installFactory(new Aline::Tests::ReferingEditableItemFactory());

	bool dummy1_ok = itemManager->createItem(Aline::Tests::DummyEditableItem::TypeId, "dummy1");
	bool dummy2_ok = itemManager->createItem(Aline::Tests::DummyEditableItem::TypeId, "dummy2");

	bool refering_ok = itemManager->createItem(Aline::Tests::ReferingEditableItem::TypeId, "refering");

	bool dummyrich1_ok = itemManager->createItem(Aline::Tests::DummyEnrichedEditableItem::TypeId, "dummyRich1");
	bool dummyrich2_ok = itemManager->createItem(Aline::Tests::DummyEnrichedEditableItem::TypeId, "dummyRich2");

	QVERIFY2(dummy1_ok, "Dummy 1 not created");
	QVERIFY2(dummy2_ok, "Dummy 2 not created");

	QVERIFY2(refering_ok, "Refering not created");

	QVERIFY2(dummyrich1_ok, "Dummy rich 1 not created");
	QVERIFY2(dummyrich2_ok, "Dummy rich 2 not created");

	dummy1 = itemManager->loadItem("dummy1");
	dummy2 = itemManager->loadItem("dummy2");

	refering = qobject_cast<Aline::Tests::ReferingEditableItem*>(itemManager->loadItem("refering"));

	dummyRich1 = qobject_cast<Aline::Tests::DummyEnrichedEditableItem*>(itemManager->loadItem("dummyRich1"));
	dummyRich2 = qobject_cast<Aline::Tests::DummyEnrichedEditableItem*>(itemManager->loadItem("dummyRich2"));

	QVERIFY2(dummy1 != nullptr, "Dummy 1 not loaded");
	QVERIFY2(dummy2 != nullptr, "Dummy 2 not loaded");

	QVERIFY2(refering != nullptr, "Refering not loaded");

	QVERIFY2(dummyRich1 != nullptr, "Dummy rich not loaded");
	QVERIFY2(dummyRich2 != nullptr, "Dummy rich not loaded");
}

void TestEditableItemLifeCycle::refSet() {

	refering->setReferedItemRef("");

	QVERIFY2(dummy1->listReferents().isEmpty(), "dummy1 referent list is not empty");
	QVERIFY2(dummy2->listReferents().isEmpty(), "dummy2 referent list is not empty");

	refering->setReferedItemRef(dummy1->getRef());

	QVERIFY2(refering->referedItemRef() == dummy1->getRef(), "Refered item could not be set");

	QVERIFY2(dummy1->listReferents().size() == 1, "dummy1 referent list do not contain a single element");
	QVERIFY2(dummy1->listReferents().first() == refering->getRef(), "dummy1 referent list do not contain the refering item");
	QVERIFY2(dummy2->listReferents().isEmpty(), "dummy2 referent list is not empty");

}
void TestEditableItemLifeCycle::refChange() {

	refering->setReferedItemRef(dummy1->getRef());

	QString oldRef = dummy1->getRef();
	QString tmpRef = "tmp";

	dummy1->changeRef(tmpRef);

	QVERIFY2(dummy1->getRef() == tmpRef, "dummy1 ref could not be changed");
	QVERIFY2(refering->referedItemRef() == dummy1->getRef(), "Refered item ref did not change");

	dummy1->changeRef(oldRef);

	QVERIFY2(dummy1->getRef() == oldRef, "dummy1 ref could not be reset");
	QVERIFY2(refering->referedItemRef() == dummy1->getRef(), "Refered item ref did not change");


}
void TestEditableItemLifeCycle::referingChange() {

	refering->setReferedItemRef(dummy1->getRef());

	QVERIFY2(refering->referedItemRef() == dummy1->getRef(), "Refered item could not be set");

	QVERIFY2(dummy1->listReferents().size() == 1, "dummy1 referent list do not contain a single element");
	QVERIFY2(dummy1->listReferents().first() == refering->getRef(), "dummy1 referent list do not contain the refering item");
	QVERIFY2(dummy2->listReferents().isEmpty(), "dummy2 referent list is not empty");

	refering->setReferedItemRef(dummy2->getRef());

	QVERIFY2(refering->referedItemRef() == dummy2->getRef(), "Refered item ref did not change");

	QVERIFY2(dummy2->listReferents().size() == 1, "dummy2 referent list do not contain a single element");
	QVERIFY2(dummy2->listReferents().first() == refering->getRef(), "dummy2 referent list do not contain the refering item");
	QVERIFY2(dummy1->listReferents().isEmpty(), "dummy1 referent list is not empty");

	refering->setReferedItemRef(dummy1->getRef());

	QVERIFY2(refering->referedItemRef() == dummy1->getRef(), "Refered item could not be set");

	QVERIFY2(dummy1->listReferents().size() == 1, "dummy1 referent list do not contain a single element");
	QVERIFY2(dummy1->listReferents().first() == refering->getRef(), "dummy1 referent list do not contain the refering item");
	QVERIFY2(dummy2->listReferents().isEmpty(), "dummy2 referent list is not empty");

}
void TestEditableItemLifeCycle::referedDeleted() {

	refering->setReferedItemRef(dummy2->getRef());

	itemManager->clearItem(dummy2->getRef());
	dummy2 = nullptr;


	QVERIFY2(refering->referedItemRef().isEmpty(), "Refered item ref did not reset");

}

void TestEditableItemLifeCycle::subItemRefered() {

	refering->setReferedItemRef(dummy1->getRef());

	QString oldRef = dummy1->getRef();
	QString tmpRef = "tmp";

	QString subRef = "sub";
	QString subTmpRef = "subtmp";

	Aline::Tests::DummyEditableItem* subitem = new Aline::Tests::DummyEditableItem(subRef, dummy1);

	refering->setReferedItemRef(subitem->getFullRefUrl());

	QVERIFY2(refering->referedItemRef() == subitem->getFullRefUrl(), "Refered item ref did not change");

	subitem->changeRef(subTmpRef);

	QVERIFY2(refering->referedItemRef() == subitem->getFullRefUrl(), "Refered item ref did not change");

	subitem->changeRef(subRef);

	QVERIFY2(refering->referedItemRef() == subitem->getFullRefUrl(), "Refered item ref did not change");

	dummy1->changeRef(tmpRef);

	QVERIFY2(refering->referedItemRef() == subitem->getFullRefUrl(), "Refered item ref did not change");

	dummy1->changeRef(oldRef);

	QVERIFY2(refering->referedItemRef() == subitem->getFullRefUrl(), "Refered item ref did not change");

	refering->setReferedItemRef(dummy1->getRef());

}

void TestEditableItemLifeCycle::editableItemJsonEncoding() {

	dummyRich1->_intData = 42;
	dummyRich1->_stringData = "Chain";
	dummyRich1->_pointFData = QPointF(0.5, -6);
	dummyRich1->_sizeData = QSizeF(42, 69);
	dummyRich1->_colorData = QColor(42, 69, 6);
	dummyRich1->_enumData = Aline::Tests::DummyEnrichedEditableItem::Foo;

	dummyRich2->_intData = 69;
	dummyRich2->_stringData = "Ball";
	dummyRich2->_pointFData = QPointF(-12, 0.876);
	dummyRich2->_sizeData = QSizeF(69, 42);
	dummyRich2->_colorData = QColor(0, 0, 0);
	dummyRich2->_enumData = Aline::Tests::DummyEnrichedEditableItem::Bar;

	QJsonObject obj = Aline::JsonUtils::encapsulateItemToJson(dummyRich1);

	QVERIFY2(obj.contains("intData"), "Int data not saved to json");
	QVERIFY2(obj.contains("stringData"), "String data not saved to json");
	QVERIFY2(obj.contains("pointFData"), "PointF data not saved to json");
	QVERIFY2(obj.contains("sizeData"), "SizeF data not saved to json");
	QVERIFY2(obj.contains("colorData"), "Color data not saved to json");
	QVERIFY2(obj.contains("enumData"), "Enum data not saved to json");

	QString strDataStr = obj.value("stringData").toString();
	QString enumDataStr = obj.value("enumData").toString();

	QCOMPARE(strDataStr, QString("Chain"));
	QCOMPARE(enumDataStr, QString("Foo"));

	Aline::JsonUtils::extractItemData(dummyRich2, obj, itemManager->factoryManager());

	QCOMPARE(dummyRich2->_intData, dummyRich1->_intData);
	QCOMPARE(dummyRich2->_stringData, dummyRich1->_stringData);
	QCOMPARE(dummyRich2->_pointFData, dummyRich1->_pointFData);
	QCOMPARE(dummyRich2->_sizeData, dummyRich1->_sizeData);
	QCOMPARE(dummyRich2->_colorData, dummyRich1->_colorData);
	QCOMPARE(dummyRich2->_enumData, dummyRich1->_enumData);

}

void TestEditableItemLifeCycle::cleanupTestCase() {

}

QTEST_MAIN(TestEditableItemLifeCycle)
#include "testeditableitemlifecycle.moc"
