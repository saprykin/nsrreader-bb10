#include <bb/cascades/Application>

#include "nsrreaderbb10.h"
#include "nsrreader.h"

#if !BBNDK_VERSION_AT_LEAST(10,3,0)
#  include "nsrsettings.h"
#endif

using namespace bb::cascades;

Q_DECL_EXPORT int main (int argc, char **argv)
{
#if !BBNDK_VERSION_AT_LEAST(10,3,0)
	if (NSRSettings::instance()->getVisualStyle () == VisualStyle::Dark)
		qputenv ("CASCADES_THEME", "dark");
	else
		qputenv ("CASCADES_THEME", "bright");
#endif

	Application app (argc, argv);

	new NSRReaderBB10 (&app);

	return Application::exec ();
}
