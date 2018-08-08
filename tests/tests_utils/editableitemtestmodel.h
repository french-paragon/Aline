#ifndef EDITABLEITEMTESTMODEL_H
#define EDITABLEITEMTESTMODEL_H

#include "aline_test_global.h"

#include "../../src/model/editableitem.h"

#include <QObject>

namespace Aline {
namespace Tests {

class TestEditableItemManager;

class ALINE_TEST_EXPORT EditableItemTestModel : public QObject
{
	Q_OBJECT
public:

protected slots:

	void initTestCase();

	/*!
	 * \brief StaticPropertiesTypes test that all stored static properties have a supported type.
	 */
	void StaticPropertiesTypes();

	/*!
	 * \brief StaticPropertiesChanges test if the item reach an unsaved state when a property is changed
	 */
	void StaticPropertiesChanges();

	void cleanupTestCase();

protected:

	EditableItem* _subject;
	TestEditableItemManager* _manager;

private:

	void recursivelyTestSubItemsStaticPropertiesTypes(EditableItem* subItem);
	void recursivelyTestSubItemsPropertiesChanges(EditableItem* subItem);
	QVariant updateValue(QVariant old);

};

} // namespace Tests
} // namespace Aline

#endif // EDITABLEITEMTESTMODEL_H
