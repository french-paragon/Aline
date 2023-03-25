#ifndef TESTEDITABLEITEMMANAGER_H
#define TESTEDITABLEITEMMANAGER_H

#include "aline_test_global.h"

#include "../../src/model/editableitemmanager.h"

namespace Aline {
namespace Tests {

/*!
 * \brief The TestEditableItemManager class manage created items like any other manager, but won't save them, instead it will just pretend it saved them. This is usefull for testing purposes, as items needs a manager to perform some tasks.
 */
class TestEditableItemManager : public EditableItemManager
{
public:
	TestEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual bool saveStruct();
	virtual bool saveLabels();
	virtual bool loadStruct();

	virtual bool isNetworkShared() const;

	virtual void reset();

protected:

	virtual EditableItem* effectivelyLoadItem(QString const& ref);

	virtual bool clearItemData(QString itemRef);

	virtual bool effectivelySaveItem(QString const& ref);

	virtual void effectivelyLoadLabels();
};

} // namespace Tests
} // namespace Aline

#endif // TESTEDITABLEITEMMANAGER_H
