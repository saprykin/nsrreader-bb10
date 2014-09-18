#include <bb/cascades/Application>

#include "nsrreaderbb10.h"
#include "nsrsettings.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main (int argc, char **argv)
{
	if (NSRSettings::instance()->getVisualStyle () == VisualStyle::Dark)
		qputenv ("CASCADES_THEME", "dark");
	else
		qputenv ("CASCADES_THEME", "bright");

	Application app (argc, argv);

	new NSRReaderBB10 (&app);

	return Application::exec ();
}
