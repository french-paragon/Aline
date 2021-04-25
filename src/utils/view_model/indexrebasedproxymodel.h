#ifndef INDEXREBASEDPROXYMODEL_H
#define INDEXREBASEDPROXYMODEL_H

#include <QAbstractItemModel>
#include <QIdentityProxyModel>

namespace Aline {

namespace ModelViewUtils {

class IndexRebasedProxyModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit IndexRebasedProxyModel(QObject *parent = 0);

	QModelIndex sourceTarget() const;
	void setSourceTarget(const QModelIndex &sourceTarget);

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual void setSourceModel(QAbstractItemModel *newSourceModel);
	virtual void setSourceModel(QAbstractItemModel *newSourceModel, QModelIndex const& targetIndex);

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:

public Q_SLOTS:

protected:

	class IntermediateModel : public QIdentityProxyModel
	{
		explicit IntermediateModel(QObject* parent);

		friend class IndexRebasedProxyModel;
	};

	void onSourceRowAboutToBeInserted(QModelIndex const& sourceParent, int first, int last);
	void onSourceColAboutToBeInserted(QModelIndex const& sourceParent, int first, int last);

	void onSourceRowInserted(QModelIndex const& sourceParent);
	void onSourceColInserted(QModelIndex const& sourceParent);


	void onSourceRowAboutToBeRemoved(QModelIndex const& sourceParent, int first, int last);
	void onSourceColAboutToBeRemoved(QModelIndex const& sourceParent, int first, int last);

	void onSourceRowRemoved(QModelIndex const& sourceParent);
	void onSourceColRemoved(QModelIndex const& sourceParent);


	void onSourceRowAboutToBeMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination, int row);
	void onSourceColAboutToBeMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination, int col);

	void onSourceRowMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination);
	void onSourceColMoved(QModelIndex const& parent, int start, int end,  QModelIndex const& destination);


	void onSourceModelDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight, QVector<int> const& roles);

	void onSourceModelAboutToReset();
	void reset();

	void checkSourceTarget();

	QAbstractItemModel * _sourceModel;
	IntermediateModel* _intermediate;


	QMetaObject::Connection _rowAboutToBeDeleteWatch;
	QMetaObject::Connection _colAboutToBeDeleteWatch;

	QMetaObject::Connection _rowDeleteWatch;
	QMetaObject::Connection _colDeleteWatch;


	QMetaObject::Connection _rowAboutToBeInsertWatch;
	QMetaObject::Connection _colAboutToBeInsertWatch;

	QMetaObject::Connection _rowInsertWatch;
	QMetaObject::Connection _colInsertWatch;


	QMetaObject::Connection _rowAboutToBeMovedWatch;
	QMetaObject::Connection _colAboutToBeMovedWatch;

	QMetaObject::Connection _rowMovedWatch;
	QMetaObject::Connection _colMovedWatch;


	QMetaObject::Connection _sourceModelAboutToResetWatch;
	QMetaObject::Connection _sourceModelResetWatch;

	QMetaObject::Connection _dataChangedWatch;

	QPersistentModelIndex _sourceTarget;

	bool _sourceWasValid;
};

}

}

#endif // INDEXREBASEDPROXYMODEL_H
