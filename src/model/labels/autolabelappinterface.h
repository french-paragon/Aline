#ifndef ALINE_AUTOLABELAPPINTERFACE_H
#define ALINE_AUTOLABELAPPINTERFACE_H

#include <QObject>
#include <QSet>

#include "./label.h"

namespace Aline {

class LabelsTree;
class EditableItemManager;

class AutoLabelAppInterface : public QObject
{
	Q_OBJECT
public:

	struct AutoLabelInfos {
		QString ref;
		QString name;
		Label::AutoLabelFunctor functor;
	};

	using Factory = std::function<AutoLabelInfos(LabelsTree*, EditableItemManager*)>;

	static const char* AutoLabelAppInterfaceCode;

	explicit AutoLabelAppInterface(QObject *parent = nullptr);

	void setupAutoLabelsInTree(LabelsTree* labelTree, EditableItemManager* project);

	void registerFactory(Factory const& factory) {
		_factories.push_back(factory);
	}

Q_SIGNALS:

protected:

	QVector<Factory> _factories;

};

} // namespace Aline

#endif // ALINE_AUTOLABELAPPINTERFACE_H
