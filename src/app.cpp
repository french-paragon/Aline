#include "app.h"

namespace Aline {

App::App(int argc, char **argv) :
	QApplication(argc, argv)
{

}

bool App::start(QString appCode) {
	_appCode = appCode;
	return true;
}

} // namespace Aline
