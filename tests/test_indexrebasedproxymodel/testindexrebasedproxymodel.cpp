#include <QTest>
#include <QStandardItemModel>

#include "../../src/utils/view_model/indexrebasedproxymodel.h"

#include <QSignalSpy>

#include <ctime>
#include <QDebug>

class testRowMotionModel : public QAbstractItemModel
{
	Q_OBJECT
public:

	explicit testRowMotionModel() {
		row1Childs << "R1C1" << "R1C2" << "R1C3";
		row2Childs << "R2C21" << "R2C2" << "R2C3";
	}

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const {

		if (parent == QModelIndex()) {
			return createIndex(row, column, nullptr);
		} else if (parent.internalPointer() == nullptr) {
			return createIndex(row, column, reinterpret_cast<quintptr>((parent.row() == 0) ? &row1Childs : &row2Childs));
		}

		return QModelIndex();

	}

	virtual QModelIndex parent(const QModelIndex &child) const {

		if (!child.isValid()) {
			return QModelIndex();
		}

		if (child.internalPointer() == nullptr) {
			return QModelIndex();
		}

		QStringList* ptr = reinterpret_cast<QStringList*>(child.internalPointer());

		if (ptr == &row1Childs) {
			return createIndex(0, 0, nullptr);
		} else if (ptr == &row2Childs) {
			return createIndex(1, 0, nullptr);
		}

		return QModelIndex();

	}

	virtual int columnCount(const QModelIndex &parent) const {

		Q_UNUSED(parent);

		return 1;
	}

	virtual int rowCount(const QModelIndex &parent) const {

		if (parent == QModelIndex()) {
			return 2;
		} else if (parent == index(0, 0, QModelIndex()) ) {
			return row1Childs.length();
		} else if (parent == index(1, 0, QModelIndex()) ) {
			return row2Childs.length();
		}

		return 0;

	}

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {

		if (role != Qt::DisplayRole) {
			return QVariant();
		}

		if (index.parent() == QModelIndex()) {

			return QString("Row%1").arg(index.row());
		}

		if (index.parent() == this->index(0, 0, QModelIndex())) {
			return row1Childs.at(index.row());
		}

		if (index.parent() == this->index(1, 0, QModelIndex())) {
			return row2Childs.at(index.row());
		}

		return QVariant();

	}

	void moveFromRow1ToRow2() {

		if (row1Childs.length() == 0) {
			return;
		}

		beginMoveRows(index(0, 0, QModelIndex()), 0, 0, index(1, 0, QModelIndex()), 0);

		QString item = row1Childs.first();
		row1Childs.pop_front();
		row2Childs.insert(0, item);

		endMoveRows();

	}

	void moveFromRow2ToRow1() {

		if (row2Childs.length() == 0) {
			return;
		}

		beginMoveRows(index(1, 0, QModelIndex()), 0, 0, index(0, 0, QModelIndex()), 0);

		QString item = row2Childs.first();
		row2Childs.pop_front();
		row1Childs.insert(0, item);

		endMoveRows();

	}

	void switchInRow1() {

		if (row1Childs.length() <= 1) {
			return;
		}

		beginMoveRows(index(0, 0, QModelIndex()), 0, 0, index(0, 0, QModelIndex()), row1Childs.length()-1);

		QString item = row1Childs.first();
		row1Childs.pop_front();
		row1Childs.append(item);

		endMoveRows();

	}

	void switchInRow2() {

		if (row2Childs.length() <= 1) {
			return;
		}

		beginMoveRows(index(1, 0, QModelIndex()), 0, 0, index(1, 0, QModelIndex()), row2Childs.length()-1);

		QString item = row2Childs.first();
		row2Childs.pop_front();
		row2Childs.append(item);

		endMoveRows();

	}

protected:

	QStringList row1Childs;
	QStringList row2Childs;

};

class IndexRebasedProxyModelTest : public QObject
{
	Q_OBJECT
public :
private Q_SLOTS:
	void initTestCase();

	void connectModel();
	void gotoSubindex();
	void externalModelResiliance();
	void resetTargetIndex();
	void testMovingIndex();
	void testInsertingVisibleChild();
	void testChangingDatas();
	void testMovingDatas();

	void cleanup();
	void cleanupTestCase();

private:

	QStandardItemModel* _sourceModel;
	QStandardItemModel* _secondSourceModel;
	Aline::ModelViewUtils::IndexRebasedProxyModel* _proxyModel;

	QStringList _list;

	QList<QMetaObject::Connection> connectionsToRemove;

};


void IndexRebasedProxyModelTest::initTestCase() {

	_list << "First" << "Second" << "Third" << "Fourth" << "Fifth";

	_sourceModel = new QStandardItemModel(this);
	_secondSourceModel = new QStandardItemModel(this);
	_proxyModel = new Aline::ModelViewUtils::IndexRebasedProxyModel(this);

	for (int i = 0; i < _list.size(); i++) {

		QStandardItem* item1 = new QStandardItem(_list[i]);
		QStandardItem* item2 = new QStandardItem(_list[_list.size()-1-i]);

		for (int j = 0; j <= i; j++) {
			item1->appendRow(new QStandardItem(_list[i] + "." + _list[j]));
		}

		for (int j = i; j < _list.size(); j++) {
			item1->appendRow(new QStandardItem(_list[i] + "." + _list[j]));
		}

		_sourceModel->appendRow(item1);
		_secondSourceModel->appendRow(item2);

	}

	srand(time(nullptr));

}

void IndexRebasedProxyModelTest::cleanup() {

	for (QMetaObject::Connection c : connectionsToRemove) {
		if (c) {
			disconnect(c);
		}
	}

	connectionsToRemove.clear();

}

void IndexRebasedProxyModelTest::cleanupTestCase() {

	for (int i = 0; i < _sourceModel->rowCount(QModelIndex()); i++) {

		QStandardItem* item = _sourceModel->item(i);

		delete item;

	}

	for (int i = 0; i < _secondSourceModel->rowCount(QModelIndex()); i++) {

		QStandardItem* item = _secondSourceModel->item(i);

		delete item;

	}

	delete _sourceModel;
	delete _secondSourceModel;

}

void IndexRebasedProxyModelTest::connectModel() {

	QSignalSpy resetBegin(_proxyModel, &QAbstractItemModel::modelAboutToBeReset);
	QSignalSpy resetEnd(_proxyModel, &QAbstractItemModel::modelReset);

	_proxyModel->setSourceModel(_sourceModel);

	QCOMPARE(resetBegin.length(), 1);
	QCOMPARE(resetEnd.length(), 1);

	QCOMPARE(_proxyModel->rowCount(), _sourceModel->rowCount());

	for (int i = 0; i < _proxyModel->rowCount(); i++) {
		QModelIndex indexSource = _sourceModel->index(i, 0);
		QModelIndex indexProxy = _proxyModel->index(i, 0);

		QCOMPARE(_proxyModel->rowCount(indexProxy), _sourceModel->rowCount(indexSource));
		QCOMPARE(_proxyModel->mapFromSource(indexSource), indexProxy);
		QCOMPARE(_proxyModel->mapToSource(indexProxy), indexSource);

		QCOMPARE(indexSource.data(), indexProxy.data());
	}

}
void IndexRebasedProxyModelTest::gotoSubindex() {

	QSignalSpy resetCount(_proxyModel, &QAbstractItemModel::modelReset);

	for (int i = 0; i < _sourceModel->rowCount(); i++) {

		QModelIndex target = _sourceModel->index(i, 0);

		_proxyModel->setSourceTarget(target);

		QCOMPARE(_sourceModel->rowCount(target), _proxyModel->rowCount());

		for (int j = 0; j < _sourceModel->rowCount(); j++) {

			QModelIndex considered = _sourceModel->index(j, 0);

			QVERIFY(!_proxyModel->mapFromSource(considered).isValid());

			for (int k = 0; k < _sourceModel->rowCount(considered); k++) {

				QModelIndex subIndex = _sourceModel->index(k, 0, considered);

				if (considered == target) {

					QCOMPARE(_proxyModel->mapFromSource(subIndex), _proxyModel->index(k, 0));
					QCOMPARE(_proxyModel->index(k, 0).data(), subIndex.data());

				} else {
					QCOMPARE(_proxyModel->mapFromSource(subIndex), QModelIndex());
				}

			}

		}

	}

	QCOMPARE(resetCount.length(), _sourceModel->rowCount());

}

void IndexRebasedProxyModelTest::externalModelResiliance() {

	int rindex = rand()%_list.size();

	QVERIFY_EXCEPTION_THROWN(_proxyModel->setSourceTarget(_secondSourceModel->index(rindex, 0)), std::invalid_argument);

}

void IndexRebasedProxyModelTest::resetTargetIndex() {

	QSignalSpy resetSpy(_proxyModel, &QAbstractItemModel::modelReset);

	_proxyModel->setSourceTarget(QModelIndex());

	QCOMPARE(resetSpy.length(), 1);
	QCOMPARE(_proxyModel->rowCount(), _sourceModel->rowCount());

	for(int i = 0; i < _list.size(); i++) {

		QModelIndex sourceIndex = _sourceModel->index(i, 0);
		QModelIndex proxyIndex = _proxyModel->index(i, 0);

		QCOMPARE(_proxyModel->mapFromSource(sourceIndex), proxyIndex);
		QCOMPARE(_proxyModel->mapToSource(proxyIndex), sourceIndex);

	}

}

void IndexRebasedProxyModelTest::testMovingIndex() {

	int count = 0;

	QMetaObject::Connection con1 = connect(_proxyModel, &QAbstractItemModel::layoutChanged, [&count] () {
		count++;
	});

	QMetaObject::Connection con2 = connect(_proxyModel, &QAbstractItemModel::rowsMoved, [&count] () {
		count += 2;
	});

	QMetaObject::Connection con3 = connect(_proxyModel, &QAbstractItemModel::dataChanged, [&count] () {
		count += 4;
	});

	QMetaObject::Connection con4 = connect(_proxyModel, &QAbstractItemModel::rowsInserted, [&count] () {
		count += 8;
	});

	QMetaObject::Connection con5 = connect(_proxyModel, &QAbstractItemModel::rowsRemoved, [&count] () {
		count += 16;
	});

	connectionsToRemove << con1 << con2 << con3 << con4 << con5;

	QModelIndex begin = _sourceModel->index(0, 0);

	_proxyModel->setSourceTarget(begin);

	QStandardItem* item = new QStandardItem("truc");
	_sourceModel->insertRow(0, item);

	QCOMPARE(_proxyModel->rowCount(), _sourceModel->rowCount(_proxyModel->sourceTarget()));

	QCOMPARE(count,  0);

	QCOMPARE(_proxyModel->sourceTarget(), _sourceModel->index(1, 0));

	for (int i = 0; i < _sourceModel->rowCount(_sourceModel->index(1, 0)); i++) {

		QModelIndex sourceIndex = _sourceModel->index(i, 0, _sourceModel->index(1, 0));
		QModelIndex proxyIndex = _proxyModel->index(i, 0);

		QCOMPARE(_proxyModel->mapFromSource(sourceIndex), proxyIndex);
		QCOMPARE(_proxyModel->mapToSource(proxyIndex), sourceIndex);

	}

	_sourceModel->removeRow(0);

}

void IndexRebasedProxyModelTest::testInsertingVisibleChild() {

	int count = 0;

	connect(_proxyModel, &QAbstractItemModel::rowsInserted, [&count] () {
		count++;
	});

	connect(_proxyModel, &QAbstractItemModel::rowsRemoved, [&count] () {
		count++;
	});

	_proxyModel->setSourceTarget(_sourceModel->index(0, 0));

	bool inserted = _sourceModel->insertRow(0, _sourceModel->index(0, 0));

	QModelIndex sourceIndex = _sourceModel->index(0, 0, _sourceModel->index(0, 0));

	_sourceModel->setData(sourceIndex, "truc");

	QModelIndex proxyIndex = _proxyModel->index(0, 0);

	QCOMPARE(sourceIndex.data().toString(), proxyIndex.data().toString());

	if (inserted) {
		QCOMPARE(_proxyModel->rowCount(), _sourceModel->rowCount(_sourceModel->index(0, 0)));
		QCOMPARE(count, 1);
	}

	bool removed = _sourceModel->removeRow(0, _sourceModel->index(0, 0));

	if (removed) {
		QCOMPARE(_proxyModel->rowCount(), _sourceModel->rowCount(_sourceModel->index(0, 0)));
		QCOMPARE(count, 2);
	}
}

void IndexRebasedProxyModelTest::testChangingDatas() {

	QSignalSpy dataChangedSpy(_proxyModel, &QAbstractItemModel::dataChanged);

	_proxyModel->setSourceTarget(_sourceModel->index(0, 0));

	QModelIndex targetSource = _sourceModel->index(0, 0, _sourceModel->index(0, 0));

	QVariant oldData = _sourceModel->data(targetSource);

	_sourceModel->setData(targetSource, oldData.toString() + "Truc.Truc");

	QCOMPARE(dataChangedSpy.length(), 1);

	targetSource = _sourceModel->index(0, 0, _sourceModel->index(0, 0));
	QModelIndex targetProxy = _proxyModel->index(0, 0);

	QCOMPARE(targetSource.data(), targetProxy.data());

	_sourceModel->setData(targetSource, oldData);

}

void IndexRebasedProxyModelTest::testMovingDatas() {

	testRowMotionModel motionTestModel;

	_proxyModel->setSourceModel(&motionTestModel);
	_proxyModel->setSourceTarget(motionTestModel.index(0, 0));

	int countRowMoved = 0;
	int countRowInserted = 0;
	int countRowRemoved = 0;

	QMetaObject::Connection con1 = connect(_proxyModel, &QAbstractItemModel::rowsMoved, [&countRowMoved] () {
		countRowMoved++;
	});

	QMetaObject::Connection con2 = connect(_proxyModel, &QAbstractItemModel::rowsInserted, [&countRowInserted] () {
		countRowInserted++;
	});

	QMetaObject::Connection con3 = connect(_proxyModel, &QAbstractItemModel::rowsRemoved, [&countRowRemoved] () {
		countRowRemoved++;
	});

	connectionsToRemove << con1 << con2 << con3;

	motionTestModel.switchInRow1();

	QCOMPARE(countRowMoved, 1);
	QCOMPARE(countRowInserted, 0);
	QCOMPARE(countRowRemoved, 0);

	motionTestModel.switchInRow2();

	QCOMPARE(countRowMoved, 1);
	QCOMPARE(countRowInserted, 0);
	QCOMPARE(countRowRemoved, 0);

	motionTestModel.moveFromRow2ToRow1();

	QCOMPARE(countRowMoved, 1);
	QCOMPARE(countRowInserted, 1);
	QCOMPARE(countRowRemoved, 0);

	motionTestModel.moveFromRow1ToRow2();

	QCOMPARE(countRowMoved, 1);
	QCOMPARE(countRowInserted, 1);
	QCOMPARE(countRowRemoved, 1);

	_proxyModel->setSourceModel(_sourceModel);

}

QTEST_MAIN(IndexRebasedProxyModelTest)
#include "testindexrebasedproxymodel.moc"
