#include <QTest>

#include <QObject>

#include "../../src/model/editableitemfactory.h"

#include "../tests_utils/testeditableitemmanager.h"
#include "../tests_utils/dummyeditableitem.h"
#include "../tests_utils/referingeditableitem.h"

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

	void cleanupTestCase();

private:

	Aline::Tests::TestEditableItemManager* itemManager;

	Aline::EditableItem* dummy1;
	Aline::EditableItem* dummy2;
	Aline::Tests::ReferingEditableItem* refering;

};

void TestEditableItemLifeCycle::initTestCase() {
	itemManager = new Aline::Tests::TestEditableItemManager(this);

	Aline::EditableItemFactoryManager* factoryManager = itemManager->factoryManager();
	factoryManager->installFactory(new Aline::Tests::DummyEditableItemFactory());
	factoryManager->installFactory(new Aline::Tests::ReferingEditableItemFactory());

	bool dummy1_ok = itemManager->createItem(Aline::Tests::DummyEditableItem::TypeId, "dummy1");
	bool dummy2_ok = itemManager->createItem(Aline::Tests::DummyEditableItem::TypeId, "dummy2");

	bool refering_ok = itemManager->createItem(Aline::Tests::ReferingEditableItem::TypeId, "refering");

	QVERIFY2(dummy1_ok, "Dummy 1 not created");
	QVERIFY2(dummy2_ok, "Dummy 1 not created");
	QVERIFY2(refering_ok, "Dummy 1 not created");

	dummy1 = itemManager->loadItem("dummy1");
	dummy2 = itemManager->loadItem("dummy2");
	refering = qobject_cast<Aline::Tests::ReferingEditableItem*>(itemManager->loadItem("refering"));

	QVERIFY2(dummy1 != nullptr, "Dummy 1 not loaded");
	QVERIFY2(dummy2 != nullptr, "Dummy 2 not loaded");
	QVERIFY2(refering != nullptr, "Refering not loaded");
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

void TestEditableItemLifeCycle::cleanupTestCase() {

}

QTEST_MAIN(TestEditableItemLifeCycle)
#include "testeditableitemlifecycle.moc"
