#include <bb/cascades/Application>

#include "nsrreaderbb10.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main (int argc, char **argv)
{
    Application app (argc, argv);

    new NSRReaderBB10 (&app);

    return Application::exec ();
}
